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

    /*fCells = (fCell*)calloc(fNumCells, sizeof(fCell));
    particles = (ParticleInfo*)calloc(maxParticles, sizeof(ParticleInfo));
    cellParticles = (cellParticle*)calloc(pNumCells + 1, sizeof(int));*/

    struct ParticleInfo : Particle {

        vec3 position;

        vec3 velocity;

        int cellParticleId;

    };

    ParticleInfo* particles;

    struct fCell {

        vec3 uvw;       // Velocity field

        vec3 duvw;      // Velocity change

        vec3 prevUVW;   // Previous velocity

        float p;

        float s;

        float particleDensity;

        int cellType;

        int numCellParticles;

        int firstCellParticle;

    };

    fCell* fCells;

    struct cellParticle {

        int num;

        int first;

    };

    cellParticle* cellParticles;

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