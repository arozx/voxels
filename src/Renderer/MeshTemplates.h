#pragma once
#include <pch.h>

namespace Engine {
    /**
     * @brief Basic vertex structure with position
     */
    struct Vertex {
        std::array<float, 3> Position;  ///< XYZ position
    };

    /**
     * @brief Collection of predefined mesh templates
     */
    namespace MeshTemplates {
        /** @brief Vertices for a basic triangle */
        static const std::vector<Vertex> Triangle = {
            Vertex{{-0.5f, -0.5f, 0.0f}},
            Vertex{{ 0.5f, -0.5f, 0.0f}},
            Vertex{{ 0.0f,  0.5f, 0.0f}}
        };

        /** @brief Indices for triangle vertices */
        static const std::vector<uint32_t> TriangleIndices = {
            0, 1, 2
        };

        /** @brief Vertices for a square */
        static const std::vector<Vertex> Square = {
            Vertex{{-0.5f, -0.5f, 0.0f}},  // bottom left
            Vertex{{ 0.5f, -0.5f, 0.0f}},  // bottom right
            Vertex{{ 0.5f,  0.5f, 0.0f}},  // top right
            Vertex{{-0.5f,  0.5f, 0.0f}}   // top left
        };

        /** @brief Indices for square vertices */
        static const std::vector<uint32_t> SquareIndices = {
            0, 1, 2,  // first triangle
            2, 3, 0   // second triangle
        };

        /** @brief Vertices for a textured square with UV coordinates */
        static const std::vector<float> TexturedSquare = {
            // positions        // texture coords
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        /**
         * @brief Converts vertex structures to raw float data
         * @param vertices Vector of vertices to convert
         * @return Vector of raw float data
         */
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
