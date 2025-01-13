#pragma once

#include <pch.h>

class VoidNoise {
public:
    VoidNoise(unsigned int seed = 1234);
    float noise(float x, float y) const;
    std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const;
private:
    static const int PERM_SIZE = 256;
    mutable std::mt19937 rng;
    std::array<int, PERM_SIZE * 2> perm;
    float fade(float t) const;
    float lerp(float a, float b, float t) const;
    float grad(int hash, float x, float y) const;
};