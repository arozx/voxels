#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Frustum {
public:
    Frustum(const glm::mat4& projection, const glm::mat4& view);

    bool isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const;

private:
    enum Planes { Near, Far, Left, Right, Top, Bottom, Count };
    glm::vec4 planes[Count];

    void normalizePlane(glm::vec4& plane);
};
