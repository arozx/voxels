#pragma once

#include <pch.h>

namespace Engine {
    /**
     * @brief Represents a 3D transformation with position, rotation, and scale
     * 
     * Provides functionality to handle 3D transformations and generate model matrices
     * for rendering purposes.
     */
    struct Transform {
        /** @brief Position in 3D space */
        glm::vec3 position = glm::vec3(0.0f);
        /** @brief Rotation in radians around each axis (x, y, z) */
        glm::vec3 rotation = glm::vec3(0.0f);
        /** @brief Scale factors for each axis */
        glm::vec3 scale = glm::vec3(1.0f);

        /**
         * @brief Generates a model matrix from the transform components
         * @return Combined transformation matrix in the order: scale -> rotate -> translate
         */
        glm::mat4 GetModelMatrix() const {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);
            return model;
        }
    };
}
