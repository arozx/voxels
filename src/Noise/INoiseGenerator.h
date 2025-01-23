
#pragma once

#include <pch.h>

class INoiseGenerator {
public:
    virtual ~INoiseGenerator() = default;
    
    /**
     * @brief Generate 2D noise value at given coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return float Noise value between 0 and 1
     */
    virtual float noise(float x, float y) const = 0;

    /**
     * @brief Generate a heightmap using multiple octaves of noise
     * @param width Width of the heightmap
     * @param height Height of the heightmap
     * @param scale Scale factor for noise coordinates
     * @return std::vector<float> Generated heightmap values
     */
    virtual std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const = 0;
};