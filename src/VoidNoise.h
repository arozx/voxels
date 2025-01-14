#pragma once

#include <pch.h>

/**
 * @brief Perlin noise implementation for terrain generation
 * @details Provides methods for generating coherent noise and heightmaps
 */
class VoidNoise {
public:
    /**
     * @brief Construct a new Void Noise generator
     * @param seed Random seed value for noise generation
     */
    VoidNoise(unsigned int seed = 1234);

    /**
     * @brief Generate 2D noise value at given coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return float Noise value between 0 and 1
     */
    float noise(float x, float y) const;

    /**
     * @brief Generate a heightmap using multiple octaves of noise
     * @param width Width of the heightmap
     * @param height Height of the heightmap
     * @param scale Scale factor for noise coordinates
     * @return std::vector<float> Generated heightmap values
     */
    std::vector<float> generateHeightmap(int width, int height, float scale = 1.0f) const;

private:
    static const int PERM_SIZE = 256;
    mutable std::mt19937 rng;
    std::array<int, PERM_SIZE * 2> perm;

    /**
     * @brief Smoothstep function for noise interpolation
     * @param t Value to fade
     * @return float Smoothed value
     */
    float fade(float t) const;

    /**
     * @brief Linear interpolation between two values
     * @param a First value
     * @param b Second value
     * @param t Interpolation factor (0-1)
     * @return float Interpolated value
     */
    float lerp(float a, float b, float t) const;

    /**
     * @brief Generate gradient value for noise
     * @param hash Hash value
     * @param x X coordinate
     * @param y Y coordinate
     * @return float Gradient value
     */
    float grad(int hash, float x, float y) const;
};