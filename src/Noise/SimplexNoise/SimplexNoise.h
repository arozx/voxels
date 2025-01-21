#pragma once

#include "../INoiseGenerator.h"

class SimplexNoise : public INoiseGenerator {
public:
    explicit SimplexNoise(unsigned int seed = 1234);
    ~SimplexNoise() override = default;

    float noise(float x, float y) const override;
    std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const override;

private:
    static float grad(int hash, float x, float y);
    static float dot2(const float* grad, float x, float y);
    
    unsigned char perm[512];
    static const float F2;
    static const float G2;
    static const int gradients[12][2];
};