#pragma once
#include <vector>
#include <array>

namespace Engine {
    struct Vertex {
        std::array<float, 3> Position;
    };

    namespace MeshTemplates {
        static const std::vector<Vertex> Triangle = {
            Vertex{{-0.5f, -0.5f, 0.0f}},
            Vertex{{ 0.5f, -0.5f, 0.0f}},
            Vertex{{ 0.0f,  0.5f, 0.0f}}
        };

        static const std::vector<uint32_t> TriangleIndices = {
            0, 1, 2
        };

        static const std::vector<Vertex> Square = {
            Vertex{{-0.5f, -0.5f, 0.0f}},  // bottom left
            Vertex{{ 0.5f, -0.5f, 0.0f}},  // bottom right
            Vertex{{ 0.5f,  0.5f, 0.0f}},  // top right
            Vertex{{-0.5f,  0.5f, 0.0f}}   // top left
        };

        static const std::vector<uint32_t> SquareIndices = {
            0, 1, 2,  // first triangle
            2, 3, 0   // second triangle
        };

        // Square vertices with texture coordinates
        static const std::vector<float> TexturedSquare = {
            // positions        // texture coords
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        // Helper function to get raw vertex data
        static std::vector<float> GetVertexData(const std::vector<Vertex>& vertices) {
            std::vector<float> data;
            data.reserve(vertices.size() * 3);
            for (const auto& vertex : vertices) {
                data.insert(data.end(), vertex.Position.begin(), vertex.Position.end());
            }
            return data;
        }
    }
}
