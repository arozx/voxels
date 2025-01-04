#include "PerlinNoise.h"
#include <algorithm>

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Initialize permutation vector with values 0-255
    p.resize(256);
    std::iota(p.begin(), p.end(), 0);
    
    // Shuffle using seed
    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);
    
    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::noise(double x, double y, double z) const {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = p[X]+Y;
    int AA = p[A]+Z;
    int AB = p[A+1]+Z;
    int B = p[X+1]+Y;
    int BA = p[B]+Z;
    int BB = p[B+1]+Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                  grad(p[BA], x-1, y, z)),
                           lerp(u, grad(p[AB], x, y-1, z),
                                  grad(p[BB], x-1, y-1, z))),
                   lerp(v, lerp(u, grad(p[AA+1], x, y, z-1),
                                  grad(p[BA+1], x-1, y, z-1)),
                           lerp(u, grad(p[AB+1], x, y-1, z-1),
                                  grad(p[BB+1], x-1, y-1, z-1))));
}

double PerlinNoise::octaveNoise(double x, double y, double z, int octaves, double persistence) const {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;

    for(int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total/maxValue;
}

double PerlinNoise::fade(double t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const {
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) const {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
