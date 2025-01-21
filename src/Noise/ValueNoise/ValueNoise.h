
#pragma once

#include "../INoiseGenerator.h"

class ValueNoise : public INoiseGenerator {
public:
    explicit ValueNoise(unsigned int seed = 1234);
    ~ValueNoise() override = default;

    float noise(float x, float y) const override;
    std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const override;

private:
    float interpolate(float a0, float a1, float w) const;
    float rand2D(int x, int y) const;
    
    unsigned int m_Seed;
    static const int PERMUTATION_SIZE = 256;
    std::vector<int> p;  // Permutation table
};