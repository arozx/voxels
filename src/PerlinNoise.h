#pragma once
#include <vector>
#include <random>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 123);
    double noise(double x, double y, double z) const;
    double octaveNoise(double x, double y, double z, int octaves, double persistence) const;

private:
    std::vector<int> p;
    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;
};
