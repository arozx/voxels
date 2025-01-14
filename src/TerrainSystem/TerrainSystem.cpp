#include <pch.h>
#include "TerrainSystem.h"
#include "Shader/DefaultShaders.h"
#include "Renderer/MeshTemplates.h"
#include "Core/AssetManager.h"

namespace Engine {
    TerrainSystem::TerrainSystem() {
        // Initialize terrain with random seed
        m_Terrain = std::make_unique<VoxelTerrain>(std::random_device{}());
        
        // Load terrain texture
        m_TerrainTexture = Texture::Create("assets/textures/kenny_simple/PNG/Orange/texture_01.png");
        
        m_TerrainShader = DefaultShaders::LoadTexturedShader();  // Using textured shader for now
        m_TerrainMaterial = std::make_shared<Material>(m_TerrainShader);
        m_TerrainMaterial->SetTexture("u_Texture", m_TerrainTexture);
        m_TerrainMaterial->SetVector4("u_Color", glm::vec4(1.0f));
        
        m_TerrainTransform.position = glm::vec3(-5.0f, -2.0f, -5.0f);
        m_TerrainTransform.scale = glm::vec3(0.2f);

        GenerateMesh();
    }

    void TerrainSystem::Initialize(Renderer& renderer) {
        // Already initialized in constructor, but could be used for renderer-specific setup
    }

    void TerrainSystem::Update(float deltaTime) {
        // Add terrain update logic here if needed
    }

    void TerrainSystem::Render(Renderer& renderer) {
        if (m_TerrainVA && m_TerrainMaterial) {
            renderer.Submit(m_TerrainVA, m_TerrainMaterial, m_TerrainTransform);
        }
    }

    void TerrainSystem::RegenerateTerrain(uint32_t seed) {
        m_Terrain = std::make_unique<VoxelTerrain>(seed);
        GenerateMesh();
    }

    void TerrainSystem::GenerateMesh() {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        uint32_t currentIndex = 0;

        // Generate mesh for each chunk in the specified range
        for (int cx = -m_ChunkRange; cx <= m_ChunkRange; cx++) {
            for (int cy = -m_ChunkRange; cy <= m_ChunkRange; cy++) {
                for (int cz = -m_ChunkRange; cz <= m_ChunkRange; cz++) {
                    // Generate chunk if it doesn't exist
                    m_Terrain->generateChunk(cx, cy, cz);
                    
                    // Get chunk data
                    const int size = VoxelChunk::CHUNK_SIZE;
                    int chunkOffsetX = cx * size;
                    int chunkOffsetY = cy * size;
                    int chunkOffsetZ = cz * size;

                    // Generate mesh for each voxel in chunk
                    for (int x = 0; x < size; x++) {
                        for (int y = 0; y < size; y++) {
                            for (int z = 0; z < size; z++) {
                                int worldX = chunkOffsetX + x;
                                int worldY = chunkOffsetY + y;
                                int worldZ = chunkOffsetZ + z;

                                if (!m_Terrain->getVoxel(worldX, worldY, worldZ)) {
                                    continue;
                                }

                                float x0 = static_cast<float>(worldX);
                                float y0 = static_cast<float>(worldY);
                                float z0 = static_cast<float>(worldZ);

                                // Top face (positive Y)
                                if (!m_Terrain->getVoxel(worldX, worldY + 1, worldZ)) {
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
                                if (!m_Terrain->getVoxel(worldX, worldY - 1, worldZ)) {
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
                                if (!m_Terrain->getVoxel(worldX, worldY, worldZ - 1)) {
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
                                if (!m_Terrain->getVoxel(worldX, worldY, worldZ + 1)) {
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
                                if (!m_Terrain->getVoxel(worldX + 1, worldY, worldZ)) {
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
                                if (!m_Terrain->getVoxel(worldX - 1, worldY, worldZ)) {
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
                }
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
    }

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
