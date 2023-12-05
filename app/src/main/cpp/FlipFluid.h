//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_FLIPFLUID_H
#define LIVEWALLPAPER05_FLIPFLUID_H


class FlipFluid {
public:

    // fluid

    float density;
    int fNumX;
    int fNumY;
    int fNumZ;  // New variable for the z-axis
    float h;
    float fInvSpacing;
    int fNumCells;

    float* u;
    float* v;
    float* w;  // New variable for the z-axis velocity
    float* du;
    float* dv;
    float* dw;  // New variable for the z-axis velocity
    float* prevU;
    float* prevV;
    float* prevW;  // New variable for the z-axis velocity
    float* p;
    float* s;
    int* cellType;

    // particles

    int maxParticles;

    vec3* particlePos;
    vec3* particleColor;

    vec3* particleVel;

    float* particleDensity;
    float particleRestDensity;

    float particleRadius;
    float pInvSpacing;
    int pNumX;
    int pNumY;
    int pNumZ;  // New variable for the z-axis
    int pNumCells;

    int* numCellParticles;
    int* firstCellParticle;
    int* cellParticleIds;

    int numParticles;

    float dt = 1.0 / 60.0;
    float flipRatio = 0.9f;
    int numPressureIters = 50;
    int numParticleIters = 2;
    int frameNr = 0;
    float overRelaxation = 1.9f;
    bool compensateDrift = true;
    bool separateParticles = true;

    FlipFluid(const float& _density, const float& _width, const float& _height, const float& _depth, const float& _spacing, const float& _particleRadius, const int& _maxParticles);

private:
    void integrateParticles(const float& _dt, const vec3& _gravity);

    void pushParticlesApart(const float& _numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(const bool& _toGrid, const float& _flipRatio);

    void solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift);

    void updateParticleColors();

public:
    void simulate(const float& _dt, const vec3& _gravity, const float& _flipRatio, const int& _numPressureIters, const int& _numParticleIters, const float& _overRelaxation, const bool& _compensateDrift, const bool& _separateParticles);
};


#endif //LIVEWALLPAPER05_FLIPFLUID_H
