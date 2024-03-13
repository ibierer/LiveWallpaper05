//
// Created by Immanuel Bierer on 1/14/2024.
//

#ifndef LIVEWALLPAPER05_NAIVESIMULATION_H
#define LIVEWALLPAPER05_NAIVESIMULATION_H


class NaiveSimulation : public Simulation {

public:

    bool seed(const int& count, const float& radius);

    void simulate(const vec3& gravity, const float& efficiency);

    struct Particle : public Simulation::Particle {
        vec3 positionFinal;
        vec3 sumForceInitial;
        vec3 sumForceFinal;
    };

    Particle* particles;

    const float deltaTime = 0.02f;

    const float surfaceTension = 0.05f;

    float sphereRadius;

    float sphereRadiusSquared;

    float sphereRadiusPlusPointFive;

    const double maxForceDistance = 2.0;

    const double maxForceDistanceSquared = maxForceDistance * maxForceDistance;

    float particleRadius;

    int* numCellParticles;

    int* firstCellParticle;

    int* cellParticleIds;

    float pInvSpacing;

    int pNumX;

    int pNumY;

    int pNumZ;

    int pNumCells;

private:

    void populateGrid(const int& dataIndex);

    void useGrid(const int& i, const int& dataIndex, int* const ids, int& count);

    float noPullBarrier(const float& x);

    float f(const float& x);

    inline void calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex);

};


#endif //LIVEWALLPAPER05_NAIVESIMULATION_H
