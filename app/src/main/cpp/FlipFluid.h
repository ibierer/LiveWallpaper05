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

    float density = 1000.0f;
    static const int width = 1000;
    static const int height = 1000;
    constexpr static float simHeight = 3.0;
    constexpr static float cScale = height / simHeight;
    constexpr static float simWidth = width / cScale;
    constexpr static float simDepth = simHeight;  // Assuming the depth is the same as the height;
    constexpr static int res = 15;
    constexpr static float spacing = simHeight / res;
    constexpr static float relWaterHeight = 1.0f;
    constexpr static float relWaterWidth = 0.6f;
    constexpr static float relWaterDepth = 1.0f;

    // compute number of particles
    constexpr static float particleRadius = 0.3 * simHeight / res;    // particle radius w.r.t. cell size;
    constexpr static float dx = 2.0 * particleRadius;
    constexpr static float dy = 1.7320508075688772935274463415059 / 2.0 * dx;
    constexpr static float dz = 2.0 * particleRadius;

    constexpr static int numX = (relWaterWidth * simWidth - 2.0 * spacing - 2.0 * particleRadius) / dx;
    constexpr static int numY = (relWaterHeight * simHeight - 2.0 * spacing - 2.0 * particleRadius) / dy;
    constexpr static int numZ = (relWaterDepth * simDepth - 2.0 * spacing - 2.0 * particleRadius) / dz;

    // fluid properties

    constexpr static int fNumX = simWidth / spacing + 1;
    constexpr static int fNumY = simHeight / spacing + 1;
    constexpr static int fNumZ = simDepth / spacing + 1;
    constexpr static float h = simWidth / fNumX;
    constexpr static const float fInvSpacing = 1.0 / h;
    constexpr static int fNumCells = fNumX * fNumY * fNumZ;
    float particleRestDensity = 0.0;                                            // Set the rest density of the particles

    constexpr static float pInvSpacing = 1.0 / (2.2 * particleRadius);
    constexpr static int pNumX = simWidth * pInvSpacing + 1;
    constexpr static int pNumY = simHeight * pInvSpacing + 1;
    constexpr static int pNumZ = simDepth * pInvSpacing + 1;
    constexpr static int pNumCells = pNumX * pNumY * pNumZ;

    static const int maxParticles = numX * numY * numZ;

    int numParticles = numX * numY * numZ;;

    float dt = 1.0 / 60.0;

    float flipRatio = 0.9f;

    int numPressureIters = 50;

    int numParticleIters = 2;

    int frameNr = 0;

    float overRelaxation = 1.9f;

    bool compensateDrift = true;

    bool separateParticles = true;

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

        ParticleInfo particles[maxParticles]; // per-particle data

        fCell fCells[fNumCells]; // velocity field cells

        pCell pCells[pNumCells + 1]; // per-cell information

    };

    Data* data;

    FlipFluid();

    void simulate(const vec3 &gravity);

private:

    static const int FLUID_CELL = 0;

    static const int AIR_CELL = 1;

    static const int SOLID_CELL = 2;

    void integrateParticles(const float &dt, const vec3 &gravity);

    void pushParticlesApart(const float& numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(const bool& toGrid, const float& flipRatio);

    void solveIncompressibility(const int& numIters, const float& dt, const float& overRelaxation, const bool& compensateDrift);

};


#endif //LIVEWALLPAPER05_FLIPFLUID_H