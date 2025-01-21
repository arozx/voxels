#include <pch.h>
#include "ValueNoise.h"

ValueNoise::ValueNoise(unsigned int seed) : m_Seed(seed) {
    // Initialize permutation table
    p.resize(PERMUTATION_SIZE);
    for(int i = 0; i < PERMUTATION_SIZE; i++) {
        p[i] = i;
    }

    std::mt19937 gen(seed);
    std::shuffle(p.begin(), p.end(), gen);
}

float ValueNoise::interpolate(float a0, float a1, float w) const {
    // Smoothstep interpolation
    return (a1 - a0) * ((w * (w * 6.0f - 15.0f) + 10.0f) * w * w * w) + a0;
}

float ValueNoise::rand2D(int x, int y) const {
    // Generate reproducible random value from coordinates and seed
    int n = x + y * 57 + m_Seed * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float ValueNoise::noise(float x, float y) const {
    int x0 = std::floor(x);
    int y0 = std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - static_cast<float>(x0);
    float sy = y - static_cast<float>(y0);

    float n0 = rand2D(x0, y0);
    float n1 = rand2D(x1, y0);
    float n2 = rand2D(x0, y1);
    float n3 = rand2D(x1, y1);

    float ix0 = interpolate(n0, n1, sx);
    float ix1 = interpolate(n2, n3, sx);
    
    return interpolate(ix0, ix1, sy);
}

std::vector<float> ValueNoise::generateHeightmap(int width, int height, float scale) const {
    std::vector<float> heightmap(width * height);
    
    const int OCTAVES = 6;
    const float PERSISTENCE = 0.5f;
    const float LACUNARITY = 2.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float total = 0.0f;
            float maxValue = 0.0f;

            for (int o = 0; o < OCTAVES; o++) {
                float sampleX = x * scale * frequency;
                float sampleY = y * scale * frequency;
                
                total += noise(sampleX, sampleY) * amplitude;
                maxValue += amplitude;
                
                amplitude *= PERSISTENCE;
                frequency *= LACUNARITY;
            }

            heightmap[y * width + x] = total / maxValue;
        }
    }
    
    return heightmap;
}