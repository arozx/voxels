
#pragma once

#include <pch.h>
#include "../INoiseGenerator.h"

class PerlinNoise : public INoiseGenerator {
public:
    PerlinNoise(unsigned int seed = 1234);

    float noise(float x, float y) const override;
    std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const override;

    void setOctaves(int octaves) { m_Octaves = octaves; }
    void setPersistence(float persistence) { m_Persistence = persistence; }
    void setLacunarity(float lacunarity) { m_Lacunarity = lacunarity; }

private:
    static const int PERM_SIZE = 256;
    std::array<int, PERM_SIZE * 2> p;
    std::array<glm::vec2, PERM_SIZE> gradients;
    
    int m_Octaves = 4;
    float m_Persistence = 0.5f;
    float m_Lacunarity = 2.0f;

    float fade(float t) const;
    float lerp(float a, float b, float t) const;
    float gradient(int hash, float x, float y) const;
    void generateGradients(unsigned int seed);
};