#include "TerrainSystem.h"
#include "Shader/DefaultShaders.h"
#include <glm/glm.hpp>

namespace Engine {
    TerrainSystem::TerrainSystem() {
        // Initialize terrain with random seed
        m_Terrain = std::make_unique<VoxelTerrain>(std::random_device{}());
        
        // Load terrain texture
        m_TerrainTexture = Texture::Create("assets/textures/kenny_simple/PNG/Orange/texture_01.png");
        
        m_TerrainShader = DefaultShaders::LoadTexturedShader();
        m_TerrainMaterial = std::make_shared<Material>(m_TerrainShader);
        m_TerrainMaterial->SetTexture("u_Texture", m_TerrainTexture);
        m_TerrainMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        
        m_TerrainTransform.position = glm::vec3(-5.0f, -2.0f, -5.0f);
        m_TerrainTransform.scale = glm::vec3(0.2f);

        GenerateMesh();
    }

    void TerrainSystem::Update(float deltaTime) {
        // Add any terrain update logic here
    }

    void TerrainSystem::Render(Renderer& renderer) {
        renderer.Submit(m_TerrainVA, m_TerrainMaterial, m_TerrainTransform);
    }

    void TerrainSystem::RegenerateTerrain(uint32_t seed) {
        m_Terrain = std::make_unique<VoxelTerrain>(seed);
        GenerateMesh();
    }

    void TerrainSystem::GenerateMesh() {
        // ...existing terrain mesh generation code from Application::GenerateTerrainMesh()...
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        uint32_t currentIndex = 0;

        const auto& voxelData = m_Terrain->getData();
        const int size = VoxelTerrain::TERRAIN_SIZE;

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                for (int z = 0; z < size; z++) {
                    if (!voxelData[x + size * (y + size * z)]) {
                        continue;
                    }

                    float x0 = static_cast<float>(x);
                    float y0 = static_cast<float>(y);
                    float z0 = static_cast<float>(z);

                    // Top face (positive Y)
                    if (y == size - 1 || !voxelData[x + size * ((y + 1) + size * z)]) {
                        vertices.insert(vertices.end(), {
                            x0, y0 + 1, z0,           0.0f, 0.0f,
                            x0, y0 + 1, z0 + 1,       0.0f, 1.0f,
                            x0 + 1, y0 + 1, z0 + 1,   1.0f, 1.0f,
                            x0 + 1, y0 + 1, z0,       1.0f, 0.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 1, currentIndex + 2,
                            currentIndex, currentIndex + 2, currentIndex + 3
                        });
                        currentIndex += 4;
                    }

                    // Bottom face (negative Y)
                    if (y == 0 || !voxelData[x + size * ((y - 1) + size * z)]) {
                        vertices.insert(vertices.end(), {
                            x0, y0, z0,           0.0f, 0.0f,
                            x0, y0, z0 + 1,       0.0f, 1.0f,
                            x0 + 1, y0, z0 + 1,   1.0f, 1.0f,
                            x0 + 1, y0, z0,       1.0f, 0.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 2, currentIndex + 1,
                            currentIndex, currentIndex + 3, currentIndex + 2
                        });
                        currentIndex += 4;
                    }

                    // Front face (negative Z)
                    if (z == 0 || !voxelData[x + size * (y + size * (z - 1))]) {
                        vertices.insert(vertices.end(), {
                            x0, y0, z0,           0.0f, 0.0f,
                            x0 + 1, y0, z0,       1.0f, 0.0f,
                            x0 + 1, y0 + 1, z0,   1.0f, 1.0f,
                            x0, y0 + 1, z0,       0.0f, 1.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 2, currentIndex + 1,
                            currentIndex, currentIndex + 3, currentIndex + 2
                        });
                        currentIndex += 4;
                    }

                    // Back face (positive Z)
                    if (z == size - 1 || !voxelData[x + size * (y + size * (z + 1))]) {
                        vertices.insert(vertices.end(), {
                            x0, y0, z0 + 1,       0.0f, 0.0f,
                            x0 + 1, y0, z0 + 1,   1.0f, 0.0f,
                            x0 + 1, y0 + 1, z0 + 1, 1.0f, 1.0f,
                            x0, y0 + 1, z0 + 1,   0.0f, 1.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 1, currentIndex + 2,
                            currentIndex, currentIndex + 2, currentIndex + 3
                        });
                        currentIndex += 4;
                    }

                    // Right face (positive X)
                    if (x == size - 1 || !voxelData[(x + 1) + size * (y + size * z)]) {
                        vertices.insert(vertices.end(), {
                            x0 + 1, y0, z0,       0.0f, 0.0f,
                            x0 + 1, y0, z0 + 1,   1.0f, 0.0f,
                            x0 + 1, y0 + 1, z0 + 1, 1.0f, 1.0f,
                            x0 + 1, y0 + 1, z0,   0.0f, 1.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 1, currentIndex + 2,
                            currentIndex, currentIndex + 2, currentIndex + 3
                        });
                        currentIndex += 4;
                    }

                    // Left face (negative X)
                    if (x == 0 || !voxelData[(x - 1) + size * (y + size * z)]) {
                        vertices.insert(vertices.end(), {
                            x0, y0, z0,           0.0f, 0.0f,
                            x0, y0, z0 + 1,       1.0f, 0.0f,
                            x0, y0 + 1, z0 + 1,   1.0f, 1.0f,
                            x0, y0 + 1, z0,       0.0f, 1.0f
                        });
                        
                        indices.insert(indices.end(), {
                            currentIndex, currentIndex + 2, currentIndex + 1,
                            currentIndex, currentIndex + 3, currentIndex + 2
                        });
                        currentIndex += 4;
                    }
                }
            }
        }

        m_TerrainVA.reset(VertexArray::Create());
        
        std::shared_ptr<VertexBuffer> vertexBuffer(
            VertexBuffer::Create(vertices.data(), 
            vertices.size() * sizeof(float)));
        
        BufferLayout layout = {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float2, "aTexCoord" }  // Add texture coordinates to layout
        };
        
        vertexBuffer->SetLayout(layout);
        m_TerrainVA->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer(
            IndexBuffer::Create(indices.data(), indices.size()));
        m_TerrainVA->SetIndexBuffer(indexBuffer);
    }
}
