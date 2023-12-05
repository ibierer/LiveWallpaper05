//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_FLIPFLUID_H
#define LIVEWALLPAPER05_FLIPFLUID_H


struct {
    const int width = 1000;
    const int height = 1000;
} canvas;

float simHeight = 3.0;
float cScale = canvas.height / simHeight;
float simWidth = canvas.width / cScale;
float simDepth = simHeight;  // Assuming the depth is the same as the height

class FlipFluid {
public:

    float density;

    int fNumX;

    int fNumY;

    int fNumZ;

    float h;

    float fInvSpacing;

    int fNumCells;

    float* u;

    float* v;

    float* w;

    float* du;

    float* dv;

    float* dw;

    float* prevU;

    float* prevV;

    float* prevW;

    float* p;

    float* s;

    int* cellType;

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

    int pNumZ;

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

    void simulate(const float& _dt, const vec3& _gravity, const float& _flipRatio, const int& _numPressureIters, const int& _numParticleIters, const float& _overRelaxation, const bool& _compensateDrift, const bool& _separateParticles);

private:

    static const int FLUID_CELL = 0;

    static const int AIR_CELL = 1;

    static const int SOLID_CELL = 2;

    void integrateParticles(const float& _dt, const vec3& _gravity);

    void pushParticlesApart(const float& _numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(const bool& _toGrid, const float& _flipRatio);

    void solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift);

    void updateParticleColors();

};


#endif //LIVEWALLPAPER05_FLIPFLUID_H
