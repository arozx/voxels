
#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Initialize permutation table
    std::vector<int> perm(PERM_SIZE);
    std::iota(perm.begin(), perm.end(), 0);
    
    std::mt19937 engine(seed);
    std::shuffle(perm.begin(), perm.end(), engine);

    // Fill permutation array
    for (int i = 0; i < PERM_SIZE; i++) {
        p[i] = p[i + PERM_SIZE] = perm[i];
    }

    generateGradients(seed);
}

void PerlinNoise::generateGradients(unsigned int seed) {
    std::mt19937 engine(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < PERM_SIZE; i++) {
        float x = dist(engine);
        float y = dist(engine);
        glm::vec2 gradient(x, y);
        gradients[i] = glm::normalize(gradient);
    }
}

float PerlinNoise::fade(float t) const {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

float PerlinNoise::gradient(int hash, float x, float y) const {
    const glm::vec2& grad = gradients[hash & (PERM_SIZE - 1)];
    return grad.x * x + grad.y * y;
}

float PerlinNoise::noise(float x, float y) const {
    // Integer coordinates
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    // Fractional coordinates
    x -= std::floor(x);
    y -= std::floor(y);

    // Fade curves
    float u = fade(x);
    float v = fade(y);

    // Hash coordinates
    int A = p[X] + Y;
    int B = p[X + 1] + Y;

    // Gradient values
    float g00 = gradient(p[A], x, y);
    float g10 = gradient(p[B], x - 1, y);
    float g01 = gradient(p[A + 1], x, y - 1);
    float g11 = gradient(p[B + 1], x - 1, y - 1);

    // Interpolate
    float result = lerp(
        lerp(g00, g10, u),
        lerp(g01, g11, u),
        v
    );

    // Scale to [0, 1] range
    return (result + 1.0f) * 0.5f;
}

std::vector<float> PerlinNoise::generateHeightmap(int width, int height, float scale) const {
    std::vector<float> heightmap(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float total = 0.0f;
            float maxValue = 0.0f;

            for (int i = 0; i < m_Octaves; i++) {
                float sampleX = x * scale * frequency / width;
                float sampleY = y * scale * frequency / height;
                
                total += noise(sampleX, sampleY) * amplitude;
                maxValue += amplitude;
                
                amplitude *= m_Persistence;
                frequency *= m_Lacunarity;
            }

            heightmap[y * width + x] = total / maxValue;
        }
    }

    return heightmap;
}