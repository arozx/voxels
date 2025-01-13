#include "VoidNoise.h"
#include <pch.h>

VoidNoise::VoidNoise(unsigned int seed) : rng(seed) {
    // Initialize array with ordered values
    std::vector<int> p(PERM_SIZE);
    std::iota(p.begin(), p.end(), 0);

    // Shuffle array
    std::shuffle(p.begin(), p.end(), rng);

    // Fill permutation table
    for (int i = 0; i < PERM_SIZE; i++) {
        perm[i] = perm[i + PERM_SIZE] = p[i];
    }
}

float VoidNoise::fade(float t) const {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float VoidNoise::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

float VoidNoise::grad(int hash, float x, float y) const {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float VoidNoise::noise(float x, float y) const {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    
    x -= std::floor(x);
    y -= std::floor(y);
    
    float u = fade(x);
    float v = fade(y);

    int A = perm[X] + Y;
    int B = perm[X + 1] + Y;
    int AA = perm[A];
    int BA = perm[B];
    int AB = perm[A + 1];
    int BB = perm[B + 1];
    
    return lerp(
        lerp(grad(perm[AA], x, y),
             grad(perm[BA], x-1, y),
             u),
        lerp(grad(perm[AB], x, y-1),
             grad(perm[BB], x-1, y-1),
             u),
        v
    ) * 0.5f + 0.5f;
}

std::vector<float> VoidNoise::generateHeightmap(int width, int height, float scale) const {
    std::vector<float> heightmap(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = static_cast<float>(x) * scale / width;
            float ny = static_cast<float>(y) * scale / height;
            
            // Use multiple octaves for more natural looking terrain
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float total = 0.0f;
            float maxValue = 0.0f;
            
            for (int i = 0; i < 4; i++) {
                total += noise(nx * frequency, ny * frequency) * amplitude;
                maxValue += amplitude;
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            
            // Normalize the result
            heightmap[y * width + x] = total / maxValue;
        }
    }
    
    return heightmap;
}