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
        m_TerrainTransform.SetPosition(-8.0f, -10.0f, -8.0f);
        m_TerrainTransform.SetScale(1.0f, 1.0f, 1.0f);

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

    /**
     * @brief Updates the terrain system state and logs terrain transform information.
     *
     * This method logs the terrain's current transform (position and scale) only once
     * during the first update cycle. Subsequent calls will not repeat the logging.
     *
     * @param deltaTime The time elapsed since the last update frame, unused in this implementation.
     *
     * @note The logging is performed at the TRACE level, providing detailed system information.
     * @note The logging occurs only once due to the static 'logged' flag.
     */
    void TerrainSystem::Update(float deltaTime) {
        static bool logged = false;
        if (!logged) {
            auto pos = m_TerrainTransform.GetPosition();
            auto scale = m_TerrainTransform.GetScale();
            LOG_TRACE_CONCAT("Terrain Transform - Position: (", pos.x, ", ", pos.y, ", ", pos.z,
                             ") Scale: (", scale.x, ", ", scale.y, ", ", scale.z, ")");
            logged = true;
        }
    }

    void TerrainSystem::Render(Renderer& renderer) {
        if (m_TerrainVA && m_TerrainMaterial) {
            renderer.Submit(m_TerrainVA, m_TerrainMaterial, m_TerrainTransform.GetModelMatrix());
        }
    }

    void TerrainSystem::RegenerateTerrain(uint32_t seed) {
        m_NoiseGen = NoiseGenerator<VoidNoise>(seed);
        GenerateMesh();
    }

    /**
     * @brief Generates a terrain mesh using procedural noise
     * 
     * Creates a detailed terrain mesh by generating vertex and index buffers
     * based on a heightmap. The method uses noise generation to create terrain
     * geometry with configurable base height, height scale, and noise scale.
     * 
     * The terrain is constructed as a grid of quads, with each vertex having
     * a calculated height based on the noise generator. Texture coordinates
     * are also generated for proper mapping.
     * 
     * @details The method performs the following key steps:
     * - Generates a heightmap using the noise generator
     * - Calculates vertex heights based on base height and height scale
     * - Creates vertices with 3D positions and 2D texture coordinates
     * - Generates indices to form triangles for rendering
     * - Creates a vertex array with vertex and index buffers
     * 
     * @note The terrain size is determined by m_ChunkRange, with a default
     * map size of (m_ChunkRange * 2 + 1) * 16
     * 
     * @warning Modifies the internal terrain vertex array (m_TerrainVA)
     * 
     * @see TerrainSystem::SetBaseHeight
     * @see TerrainSystem::SetHeightScale
     * @see TerrainSystem::SetNoiseScale
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

    /**
     * @brief Generates a terrain mesh with a specified random seed.
     *
     * This method allows regenerating the terrain mesh using a custom seed for the noise generator.
     * It sets the noise generator to a new seed and then calls the standard mesh generation method.
     *
     * @param seed The random seed used to initialize the noise generator for terrain height variation.
     *
     * @note This method provides more control over terrain generation by allowing reproducible terrain
     *       through seed specification.
     */
    void TerrainSystem::GenerateMesh(uint32_t seed) {
        // Set the seed for the noise generator
        m_NoiseGen = NoiseGenerator<VoidNoise>(seed);

        // Call the regular mesh generation
        GenerateMesh();
    }

    /**
     * @brief Sets the base height for terrain generation and regenerates the terrain mesh.
     * 
     * @param height The new base height value to be used for terrain generation.
     * 
     * @details This method updates the base height parameter of the terrain system
     * and immediately triggers a mesh regeneration to reflect the new height setting.
     * The base height affects the overall vertical positioning of the terrain.
     * 
     * @note Calling this method will cause the terrain mesh to be completely regenerated
     * with the new base height parameter.
     */
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
