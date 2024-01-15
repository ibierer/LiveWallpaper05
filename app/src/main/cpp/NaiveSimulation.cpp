//
// Created by Immanuel Bierer on 1/14/2024.
//

#include "NaiveSimulation.h"

void NaiveSimulation::setParticleCount(int n){
    count = n;
    cbrtCount = cbrt(n);
}

bool NaiveSimulation::seed(const float& radius) {
    setParticleCount(1000);
    particles = new NaiveSimulation::Particle[count];
    sphereRadius = radius;
    sphereRadiusSquared = sphereRadius * sphereRadius;
    sphereRadiusPlusPointFive = sphereRadius + 0.5f;
    sphereRadiusPlusPointFiveSquared = sphereRadiusPlusPointFive * sphereRadiusPlusPointFive;
    for (int i = 0; i < count; i++) {
        int first = i - (i % (cbrtCount * cbrtCount));
        particles[i].position.x = first / (cbrtCount * cbrtCount) - (cbrtCount - 1) / 2.0f;
        first = i - first;
        particles[i].position.y = (first - (first % cbrtCount)) / cbrtCount - (cbrtCount - 1) / 2.0f;
        particles[i].position.z = first % cbrtCount - (cbrtCount - 1) / 2.0f;
        particles[i].velocity = vec3(0.0f, 0.000001f, 0.0f);
        particles[i].positionFinal = particles[i].position;
    }

    particleRadius = 0.5f;
    pInvSpacing = 1.0 / (maxForceDistance * 2.0 * particleRadius);//4.0 correlates to max distance of 2.0, 3.0 correlates to max distance of 1.5
    float width = 2.0f * sphereRadiusPlusPointFive;
    float height = 2.0f * sphereRadiusPlusPointFive;
    float depth = 2.0f * sphereRadiusPlusPointFive;
    pNumX = floor(width * pInvSpacing) + 1;
    pNumY = floor(height * pInvSpacing) + 1;
    pNumZ = floor(depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;
    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(count, sizeof(int));

    populateGrid(0);

    return true;
}

void NaiveSimulation::simulate(const vec3& gravity) {
    for (int i = 0; i < count; i++) {
        calculateForce(i, gravity, particles[i].sumForceInitial, particles[i].position, 0);
        particles[i].positionFinal = particles[i].position + deltaTime * (particles[i].sumForceInitial * 0.5f * deltaTime + particles[i].velocity);
        // Bug fix: Stop runaway particles
        float squaredDistance = dot(particles[i].positionFinal, particles[i].positionFinal);
        if(squaredDistance > sphereRadiusPlusPointFive * sphereRadiusPlusPointFive){
            particles[i].positionFinal *= sqrt(sphereRadius * sphereRadius / squaredDistance);
        }
    }
    populateGrid(2);
    for (int i = 0; i < count; i++) {
        calculateForce(i, gravity, particles[i].sumForceFinal, particles[i].positionFinal, 2);
        vec3 sumForceAverage = 0.5f * (particles[i].sumForceInitial + particles[i].sumForceFinal);
        particles[i].position += deltaTime * (sumForceAverage * 0.5f * deltaTime + particles[i].velocity);
        particles[i].velocity += sumForceAverage * deltaTime;
        // Efficiency control
        if (efficiency != 1.0f) {
            vec3 product = particles[i].velocity * efficiency;
            particles[i].velocity = product;
        }
        //  Bug fix: stop runaway particles
        float squaredDistance = dot(particles[i].position, particles[i].position);
        if(squaredDistance > sphereRadiusSquared){
            particles[i].position *= sqrt(sphereRadiusSquared / squaredDistance);
            particles[i].velocity = 0.1f * particles[i].position;
        }
    }
    populateGrid(0);
}
