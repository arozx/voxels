#include "Frustum.h"

Frustum::Frustum(const glm::mat4& projection, const glm::mat4& view) {
    glm::mat4 clip = projection * view;

    planes[Near] = glm::vec4(clip[0][3] + clip[0][2], clip[1][3] + clip[1][2], clip[2][3] + clip[2][2], clip[3][3] + clip[3][2]);
    planes[Far] = glm::vec4(clip[0][3] - clip[0][2], clip[1][3] - clip[1][2], clip[2][3] - clip[2][2], clip[3][3] - clip[3][2]);
    planes[Left] = glm::vec4(clip[0][3] + clip[0][0], clip[1][3] + clip[1][0], clip[2][3] + clip[2][0], clip[3][3] + clip[3][0]);
    planes[Right] = glm::vec4(clip[0][3] - clip[0][0], clip[1][3] - clip[1][0], clip[2][3] - clip[2][0], clip[3][3] - clip[3][0]);
    planes[Top] = glm::vec4(clip[0][3] - clip[0][1], clip[1][3] - clip[1][1], clip[2][3] - clip[2][1], clip[3][3] - clip[3][1]);
    planes[Bottom] = glm::vec4(clip[0][3] + clip[0][1], clip[1][3] + clip[1][1], clip[2][3] + clip[2][1], clip[3][3] + clip[3][1]);

    for (int i = 0; i < Count; ++i) {
        normalizePlane(planes[i]);
    }
}

void Frustum::normalizePlane(glm::vec4& plane) {
    float length = glm::length(glm::vec3(plane));
    plane /= length;
}

bool Frustum::isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const {
    for (int i = 0; i < Count; ++i) {
        const glm::vec4& plane = planes[i];
        glm::vec3 positive = min;
        if (plane.x >= 0) positive.x = max.x;
        if (plane.y >= 0) positive.y = max.y;
        if (plane.z >= 0) positive.z = max.z;

        if (glm::dot(glm::vec3(plane), positive) + plane.w < 0) {
            return false;
        }
    }
    return true;
}
