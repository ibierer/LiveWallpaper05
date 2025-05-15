//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_FLIPFLUID_H
#define LIVEWALLPAPER05_FLIPFLUID_H


#include "vectors.h"

using std::min;
using std::max;

class FlipFluid : Simulation {
public:

    float density;

    int fNumX;

    int fNumY;

    int fNumZ;

    float h;

    float fInvSpacing;

    int fNumCells;

    struct ParticleInfo : Particle {

        int cellParticleId;

    };

    struct fCell {

        vec3 uvw;       // Combined velocity field

        vec3 duvw;      // Combined velocity change

        vec3 prevUVW;   // Combined previous velocity

        float p;

        float s;

        int cellType;

        float particleDensity;

    };

    struct pCell {

        int numCellParticles; // Max = 6

        int firstCellParticle;

    };

    struct Data {

        ParticleInfo* particles;

        fCell* fCells;

        pCell* pCells;

    };

    Data data;

    int maxParticles;

    float particleRestDensity;

    float particleRadius;

    float pInvSpacing;

    int pNumX;

    int pNumY;

    int pNumZ;

    int pNumCells;

    int numParticles;

    float dt = 1.0 / 60.0;

    float flipRatio = 0.9f;

    int numPressureIters = 50;

    int numParticleIters = 2;

    int frameNr = 0;

    float overRelaxation = 1.9f;

    bool compensateDrift = true;

    bool separateParticles = true;

    FlipFluid();

    void simulate(const vec3 &_gravity);

private:

    static const int FLUID_CELL = 0;

    static const int AIR_CELL = 1;

    static const int SOLID_CELL = 2;

    void integrateParticles(const float &_dt, const vec3 &_gravity);

    void pushParticlesApart(const float& _numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(const bool& _toGrid, const float& _flipRatio);

    void solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift);

};


#endif //LIVEWALLPAPER05_FLIPFLUID_H