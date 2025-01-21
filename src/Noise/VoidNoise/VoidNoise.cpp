#include "Noise/VoidNoise/VoidNoise.h"
#include <pch.h>

/**
 * @brief Initialize the Perlin noise generator
 * @param seed Random seed value for noise generation
 */
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

/**
 * @brief Apply smoothstep function for noise interpolation
 * @param t Input value to be smoothed (0-1)
 * @return Smoothed value
 */
float VoidNoise::fade(float t) const {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/**
 * @brief Linear interpolation between two values
 * @param a First value
 * @param b Second value
 * @param t Interpolation factor (0-1)
 * @return Interpolated value
 */
float VoidNoise::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

/**
 * @brief Generate gradient value for noise calculation
 * @param hash Hash value for pseudo-random direction
 * @param x X coordinate
 * @param y Y coordinate
 * @return Gradient value
 */
float VoidNoise::grad(int hash, float x, float y) const {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

/**
 * @brief Generate 2D Perlin noise value
 * @param x X coordinate in noise space
 * @param y Y coordinate in noise space
 * @return Noise value between 0 and 1
 */
float VoidNoise::noise(float x, float y) const {
    // Get integer coordinates
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    
    // Get fractional part
    x -= std::floor(x);
    y -= std::floor(y);
    
    // Compute fade curves
    float u = fade(x);
    float v = fade(y);

    // Hash coordinates
    int A = (perm[X] + Y) & 255;
    int B = (perm[X + 1] + Y) & 255;
    
    // Get corner gradients
    float g00 = perm[A] / 255.0f;
    float g10 = perm[B] / 255.0f;
    float g01 = perm[A + 1] / 255.0f;
    float g11 = perm[B + 1] / 255.0f;

    // Interpolate between gradients
    float result = lerp(
        lerp(g00, g10, u),
        lerp(g01, g11, u),
        v
    );

    // Debug output for first few calls
    static int debugCount = 0;
    if (debugCount < 5) {
        LOG_TRACE_CONCAT("VoidNoise - Input (",x,",",y,"):", 
                        " Corners: ", g00, ",", g10, ",", g01, ",", g11,
                        " Result: ", result);
        debugCount++;
    }

    return result;
}

/**
 * @brief Generate a heightmap using multiple octaves of noise
 * @param width Width of the heightmap
 * @param height Height of the heightmap
 * @param scale Scale factor for noise coordinates
 * @return Vector containing heightmap values
 */
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

            // Debug first few values
            if (x == 0 && y < 2) {
                LOG_TRACE_CONCAT("Heightmap value at (", x, ",", y, "): ", heightmap[y * width + x]);
            }
        }
    }
    
    return heightmap;
}