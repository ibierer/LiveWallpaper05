//
// Created by Immanuel Bierer on 1/14/2024.
//

#include "NaiveSimulation.h"

using std::min;
using std::max;

bool NaiveSimulation::seed(const int& count, const float& radius) {
    setParticleCount(count);
    particles = new NaiveSimulation::Particle[particleCount];
    sphereRadius = radius;
    sphereRadiusSquared = sphereRadius * sphereRadius;
    sphereRadiusPlusPointFive = sphereRadius + 0.5f;
    for (int i = 0; i < particleCount; i++) {
        int first = i - (i % (cbrtParticleCount * cbrtParticleCount));
        particles[i].position.x = first / (cbrtParticleCount * cbrtParticleCount) - (cbrtParticleCount - 1) / 2.0f;
        first = i - first;
        particles[i].position.y = (first - (first % cbrtParticleCount)) / cbrtParticleCount - (cbrtParticleCount - 1) / 2.0f;
        particles[i].position.z = first % cbrtParticleCount - (cbrtParticleCount - 1) / 2.0f;
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
    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of stars in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(particleCount, sizeof(int));

    populateGrid(0);

    return true;
}

void NaiveSimulation::simulate(const vec3& gravity, const float& efficiency) {
    for (int i = 0; i < particleCount; i++) {
        calculateForce(i, gravity, particles[i].sumForceInitial, particles[i].position, 0);
        particles[i].positionFinal = particles[i].position + deltaTime * (particles[i].sumForceInitial * 0.5f * deltaTime + particles[i].velocity);
        // Bug fix: Stop runaway stars
        float squaredDistance = dot(particles[i].positionFinal, particles[i].positionFinal);
        if(squaredDistance > sphereRadiusPlusPointFive * sphereRadiusPlusPointFive){
            particles[i].positionFinal *= sqrt(sphereRadius * sphereRadius / squaredDistance);
        }
    }
    populateGrid(2);
    for (int i = 0; i < particleCount; i++) {
        calculateForce(i, gravity, particles[i].sumForceFinal, particles[i].positionFinal, 2);
        vec3 sumForceAverage = 0.5f * (particles[i].sumForceInitial + particles[i].sumForceFinal);
        particles[i].position += deltaTime * (sumForceAverage * 0.5f * deltaTime + particles[i].velocity);
        particles[i].velocity += sumForceAverage * deltaTime;
        // Efficiency control
        if (efficiency != 1.0f) {
            vec3 product = particles[i].velocity * efficiency;
            particles[i].velocity = product;
        }
        //  Bug fix: stop runaway stars
        float squaredDistance = dot(particles[i].position, particles[i].position);
        if(squaredDistance > sphereRadiusSquared){
            particles[i].position *= sqrt(sphereRadiusSquared / squaredDistance);
            particles[i].velocity = 0.1f * particles[i].position;
        }
    }
    populateGrid(0);
}

void NaiveSimulation::populateGrid(const int& dataIndex){
    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < particleCount; i++) {
        float x = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        numCellParticles[cellNr]++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += numCellParticles[i];
        firstCellParticle[i] = first;
    }
    firstCellParticle[pNumCells] = first; // guard

    // fill stars into cells

    for (int i = 0; i < particleCount; i++) {
        float x = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }
}

void NaiveSimulation::useGrid(const int& i, const int& dataIndex, int* const ids, int& count){
    count = 0;
    float px = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
    float py = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
    float pz = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

    int pxi = floor(px * pInvSpacing);
    int pyi = floor(py * pInvSpacing);
    int pzi = floor(pz * pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, pNumX - 1);
    int y1 = min(pyi + 1, pNumY - 1);
    int z1 = min(pzi + 1, pNumZ - 1);

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
                int first = firstCellParticle[cellNr];
                int last = firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = cellParticleIds[j];
                    if (id == i)
                        continue;
                    ids[count++] = id;
                }
            }
        }
    }
}

float NaiveSimulation::noPullBarrier(const float& x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    return 1000.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
}

float NaiveSimulation::f(const float& x) {
    if (x > maxForceDistance) {
        return 0.0f;
    }
    float _return = 500.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
    if (x > 1.0f) {
        return surfaceTension * _return;
    } else {
        return _return;
    }
}

inline void NaiveSimulation::calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex){
    force = gravity;
    int ids[1000]; // 700 may be high enough
    int count = 0;
    useGrid(i, dataIndex, ids, count);
    for (int j = 0; j < count; j++) {
        if (i == j) {
            continue;
        }
        vec3 difference = ((vec3*)&particles[ids[j]])[dataIndex] - position;
        float r = distance(difference);
        float fOverR = f(r) / r;
        force += fOverR * difference;
    }
    float distanceFromOrigin = distance(position);
    if (distanceFromOrigin != 0.0f) {
        float forceAtPoint = noPullBarrier(-distanceFromOrigin + sphereRadiusPlusPointFive) / distanceFromOrigin;//if distanceFromOrigin = 0, then we plug in a value of 3.5, if distanceFromOrigin = 2.5, then we plug in a value of 1
        force += forceAtPoint * position;
    }
}
