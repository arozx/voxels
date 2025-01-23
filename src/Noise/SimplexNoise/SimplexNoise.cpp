#include <pch.h>
#include "SimplexNoise.h"

// Initialize static constants
const float SimplexNoise::F2 = 0.366025403784439f;  // (sqrt(3) - 1) / 2
const float SimplexNoise::G2 = 0.211324865405187f;  // (3 - sqrt(3)) / 6

const int SimplexNoise::gradients[12][2] = {
    {1,1}, {-1,1}, {1,-1}, {-1,-1},
    {1,0}, {-1,0}, {1,0}, {-1,0},
    {0,1}, {0,-1}, {0,1}, {0,-1}
};

SimplexNoise::SimplexNoise(unsigned int seed) {
    std::vector<unsigned char> p(256);
    for(int i = 0; i < 256; i++) {
        p[i] = static_cast<unsigned char>(i);
    }
    
    std::mt19937 gen(seed);
    std::shuffle(p.begin(), p.end(), gen);

    // Duplicate the permutation table to avoid overflow
    for(int i = 0; i < 512; i++) {
        perm[i] = p[i & 255];
    }
}

float SimplexNoise::dot2(const float* grad, float x, float y) {
    return grad[0] * x + grad[1] * y;
}

float SimplexNoise::grad(int hash, float x, float y) {
    const int h = hash & 11;
    return dot2(reinterpret_cast<const float*>(gradients[h]), x, y);
}

float SimplexNoise::noise(float x, float y) const {
    float s = (x + y) * F2;
    int i = std::floor(x + s);
    int j = std::floor(y + s);

    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;
    float x0 = x - X0;
    float y0 = y - Y0;

    int i1, j1;
    if(x0 > y0) {
        i1 = 1;
        j1 = 0;
    } else {
        i1 = 0;
        j1 = 1;
    }

    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;

    int gi0 = perm[(i + perm[j & 255]) & 255] % 12;
    int gi1 = perm[(i + i1 + perm[(j + j1) & 255]) & 255] % 12;
    int gi2 = perm[(i + 1 + perm[(j + 1) & 255]) & 255] % 12;

    float n0, n1, n2;

    float t0 = 0.5f - x0*x0 - y0*y0;
    if(t0 < 0) n0 = 0.0f;
    else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    float t1 = 0.5f - x1*x1 - y1*y1;
    if(t1 < 0) n1 = 0.0f;
    else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    float t2 = 0.5f - x2*x2 - y2*y2;
    if(t2 < 0) n2 = 0.0f;
    else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    return 70.0f * (n0 + n1 + n2);
}

std::vector<float> SimplexNoise::generateHeightmap(int width, int height, float scale) const {
    std::vector<float> heightmap(width * height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = noise(x * scale, y * scale);
            heightmap[y * width + x] = (value + 1.0f) * 0.5f;
        }
    }
    
    return heightmap;
}