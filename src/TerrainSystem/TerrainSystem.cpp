/**
 * @file TerrainSystem.cpp
 * @brief Implementation of the voxel terrain system
 *
 * Provides implementation for terrain generation, mesh creation,
 * and rendering of voxel-based terrain.
 */
#include "TerrainSystem.h"

#include <pch.h>

#include "BlockTypes.h"
#include "Core/AssetManager.h"
#include "Renderer/MeshTemplates.h"
#include "Shader/DefaultShaders.h"

namespace Engine {
    /**
     * @brief Constructs terrain system with default parameters
     * 
     * Initializes terrain with random seed, loads textures and shaders,
     * and generates initial terrain mesh.
     */
    TerrainSystem::TerrainSystem() 
        : m_NoiseGen(std::random_device{}())
    {
        // Load terrain texture
        m_TerrainTexture = Texture::Create("assets/textures/terrain_atlas.png");
        
        m_TerrainShader = DefaultShaders::LoadTexturedShader();
        m_TerrainMaterial = std::make_shared<Material>(m_TerrainShader);
        m_TerrainMaterial->SetTexture("u_Texture", m_TerrainTexture);
        m_TerrainMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        
        // Update transform to better initial values
        m_TerrainTransform.position = glm::vec3(-8.0f, -10.0f, -8.0f);
        m_TerrainTransform.scale = glm::vec3(1.0f);

        // Set default terrain parameters
        m_BaseHeight = 0.0f;
        m_HeightScale = 20.0f;
        m_NoiseScale = 4.0f;

        GenerateMesh();
    }

    // Core functionality
    void TerrainSystem::Initialize(Renderer& renderer) {
        // Already initialized in constructor, but could be used for renderer-specific setup
    }

    void TerrainSystem::Update(float deltaTime) {
        static bool logged = false;
        if (!logged) {
            LOG_TRACE_CONCAT("Terrain Transform - Position: (", m_TerrainTransform.position.x, ", ",
                             m_TerrainTransform.position.y, ", ", m_TerrainTransform.position.z,
                             ") Scale: (", m_TerrainTransform.scale.x, ", ",
                             m_TerrainTransform.scale.y, ", ", m_TerrainTransform.scale.z, ")");
            logged = true;
        }
    }

    void TerrainSystem::Render(Renderer& renderer) {
        if (m_TerrainVA && m_TerrainMaterial) {
            renderer.Submit(m_TerrainVA, m_TerrainMaterial, m_TerrainTransform);
        }
    }

    void TerrainSystem::RegenerateTerrain(uint32_t seed) {
        m_NoiseGen = NoiseGenerator<VoidNoise>(seed);
        GenerateMesh();
    }

    /**
     * @brief Generates terrain mesh for all visible chunks
     * 
     * Creates vertex and index buffers for terrain geometry,
     * handling face culling and texture coordinates.
     */
    void TerrainSystem::GenerateMesh() {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        uint32_t currentIndex = 0;

        // Increase map size for better visibility
        int mapSize = (m_ChunkRange * 2 + 1) * 16;
        auto heightmap = m_NoiseGen.getHeightmap(mapSize, mapSize, m_NoiseScale);

        // Generate quad mesh with proper UVs
        for (int z = 0; z < mapSize - 1; z++) {
            for (int x = 0; x < mapSize - 1; x++) {
                float height00 = m_BaseHeight + heightmap[z * mapSize + x] * m_HeightScale;
                float height10 = m_BaseHeight + heightmap[z * mapSize + (x + 1)] * m_HeightScale;
                float height01 = m_BaseHeight + heightmap[(z + 1) * mapSize + x] * m_HeightScale;
                float height11 = m_BaseHeight + heightmap[(z + 1) * mapSize + (x + 1)] * m_HeightScale;

                // Add vertices for this quad
                vertices.insert(vertices.end(), {
                    static_cast<float>(x), height00, static_cast<float>(z),         0.0f, 0.0f,
                    static_cast<float>(x + 1), height10, static_cast<float>(z),     1.0f, 0.0f,
                    static_cast<float>(x + 1), height11, static_cast<float>(z + 1), 1.0f, 1.0f,
                    static_cast<float>(x), height01, static_cast<float>(z + 1),     0.0f, 1.0f
                });

                // Add indices for the quad (two triangles)
                indices.insert(indices.end(), {
                    currentIndex, currentIndex + 1, currentIndex + 2,
                    currentIndex, currentIndex + 2, currentIndex + 3
                });

                currentIndex += 4;
            }
        }

        // Create vertex array with mesh data
        m_TerrainVA.reset(VertexArray::Create());
        
        if (!vertices.empty()) {
            std::shared_ptr<VertexBuffer> vertexBuffer(
                VertexBuffer::Create(vertices.data(), 
                vertices.size() * sizeof(float)));
            
            BufferLayout layout = {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aTexCoord" }
            };
            
            vertexBuffer->SetLayout(layout);
            m_TerrainVA->AddVertexBuffer(vertexBuffer);

            std::shared_ptr<IndexBuffer> indexBuffer(
                IndexBuffer::Create(indices.data(), indices.size()));
            m_TerrainVA->SetIndexBuffer(indexBuffer);
        }

        // Debug output
        LOG_TRACE_CONCAT("Generated terrain mesh with ", vertices.size() / 5, " vertices and ",
                         indices.size(), " indicies.");
    }

    void TerrainSystem::GenerateMesh(uint32_t seed) {
        // Set the seed for the noise generator
        m_NoiseGen = NoiseGenerator<VoidNoise>(seed);

        // Call the regular mesh generation
        GenerateMesh();
    }

    // Parameter setters with mesh regeneration
    void TerrainSystem::SetBaseHeight(float height) {
        m_BaseHeight = height;
        GenerateMesh();
    }

    void TerrainSystem::SetHeightScale(float scale) {
        m_HeightScale = scale;
        GenerateMesh();
    }

    void TerrainSystem::SetNoiseScale(float scale) {
        m_NoiseScale = scale;
        GenerateMesh();
    }
}
