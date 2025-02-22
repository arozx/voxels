#pragma once
#include <pch.h>

namespace Engine {
class Transform {
   public:
    struct TransformData {
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
    };

    Transform() = default;

    TransformData& GetData() { return m_Data; }
    const TransformData& GetData() const { return m_Data; }

    // Position methods
    void SetPosition(float x, float y, float z) { m_Data.position = {x, y, z}; }
    void SetPosition(const glm::vec3& position) { m_Data.position = position; }
    glm::vec3 GetPosition() const { return m_Data.position; }

    // Rotation methods
    void SetRotation(float x, float y, float z) { m_Data.rotation = {x, y, z}; }
    void SetRotation(const glm::vec3& rotation) { m_Data.rotation = rotation; }
    glm::vec3 GetRotation() const { return m_Data.rotation; }

    // Scale methods
    void SetScale(float x, float y, float z) { m_Data.scale = {x, y, z}; }
    void SetScale(const glm::vec3& scale) { m_Data.scale = scale; }
    glm::vec3 GetScale() const { return m_Data.scale; }

    // Single GetModelMatrix implementation
    glm::mat4 GetModelMatrix() const {
        if (m_Data.scale == glm::vec3(0.0f)) {
            return glm::mat4(1.0f);  // Return identity if invalid scale
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Data.position);
        model = glm::rotate(model, glm::radians(m_Data.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Data.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Data.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_Data.scale);
        return model;
    }

   private:
    TransformData m_Data;
};
}  // namespace Engine