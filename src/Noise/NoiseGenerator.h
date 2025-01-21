
#pragma once

#include <pch.h>
#include "INoiseGenerator.h"

template<typename T>
class NoiseGenerator {
public:
    NoiseGenerator(unsigned int seed = 1234) : generator(seed) {}

    float getNoise(float x, float y) const {
        return generator.noise(x, y);
    }

    std::vector<float> getHeightmap(int width, int height, float scale = 1.0f) const {
        return generator.generateHeightmap(width, height, scale);
    }

private:
    T generator;
};