//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "FlipFluid.h"

FlipFluid::FlipFluid() {

    data = new Data;
    ALOGI("fNumCells = %d\n", fNumCells);
    ALOGI("sizeof(data->fCells) = %d\n", fNumCells * sizeof(fCell));
    ALOGI("pNumCells = %d\n", pNumCells);
    ALOGI("sizeof(data->pCells) = %d\n", (pNumCells + 1) * sizeof(pCell));
    ALOGI("maxParticles = %d\n", maxParticles);
    ALOGI("sizeof(data->particles) = %d\n", maxParticles * sizeof(ParticleInfo));
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            for (int k = 0; k < numZ; k++) {
                data->particles[(i * numY + j) * numZ + k].position = vec3(
                        spacing + particleRadius + dx * i + (j % 2 == 0 ? 0.0 : particleRadius),
                        spacing + particleRadius + dy * j,
                        spacing + dz * k
                );
            }
        }
    }

    // setup grid cells for tank

    for (int i = 0; i < fNumX; i++) {
        for (int j = 0; j < fNumY; j++) {
            for (int k = 0; k < fNumZ; k++) {
                float cellDensity = 1.0;    // fluid
                if (i == 0 || i == fNumX - 1 || j == 0 || j == fNumY - 1 || k == 0 || k == fNumZ - 1)
                    cellDensity = 0.0;    // solid
                data->fCells[(i * fNumY + j) * fNumZ + k].s = cellDensity;
            }
        }
    }
}
// Iterate over particles
void FlipFluid::integrateParticles(const float &dt, const vec3 &gravity)
{
    for (int i = 0; i < numParticles; i++) {
        data->particles[i].velocity += dt * gravity;
        data->particles[i].position += data->particles[i].velocity * dt;
    }
}
// Thrashing issue: reading and writing from and to pCells and particles. Non-parallelizable issue
void FlipFluid::pushParticlesApart(const float& numIters)
{
    float colorDiffusionCoeff = 0.001;

    // particleCount particles per cell

    for (int i = 0; i < pNumCells; i++)
        data->pCells[i].numCellParticles = 0;

    for (int i = 0; i < numParticles; i++) {
        int xi = clamp(floor(data->particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(data->particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(data->particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
        data->pCells[cellNr].numCellParticles++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += data->pCells[i].numCellParticles;
        data->pCells[i].firstCellParticle = first;
    }
    data->pCells[pNumCells].firstCellParticle = first; // guard

    // fill particles into cells

    for (int i = 0; i < numParticles; i++) {
        int xi = clamp(floor(data->particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(data->particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(data->particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
        data->pCells[cellNr].firstCellParticle--;
        data->particles[data->pCells[cellNr].firstCellParticle].cellParticleId = i;
    }

    // push particles apart

    float minDist = 2.0 * particleRadius;
    float minDist2 = minDist * minDist;

    for (int iter = 0; iter < numIters; iter++) {

        for (int i = 0; i < numParticles; i++) {
            vec3 pxyz = data->particles[i].position;
            int pxi = floor(data->particles[i].position.x * pInvSpacing);
            int pyi = floor(data->particles[i].position.y * pInvSpacing);
            int pzi = floor(data->particles[i].position.z * pInvSpacing);
            int x0 = max(pxi - 1, 0);
            int y0 = max(pyi - 1, 0);
            int z0 = max(pzi - 1, 0);
            int x1 = min(pxi + 1, pNumX - 1);
            int y1 = min(pyi + 1, pNumY - 1);
            int z1 = min(pzi + 1, pNumZ - 1);

            for (int xi = x0; xi <= x1; xi++) {
                for (int yi = y0; yi <= y1; yi++) {
                    for (int zi = z0; zi <= z1; zi++) {
                        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
                        first = data->pCells[cellNr].firstCellParticle;
                        int last = data->pCells[cellNr + 1].firstCellParticle;
                        for (int j = first; j < last; j++) {
                            int id = data->particles[j].cellParticleId;
                            if (id == i)
                                continue;
                            vec3 dxyz = data->particles[id].position - pxyz;
                            float d2 = dot(dxyz, dxyz);
                            if (d2 > minDist2 || d2 == 0.0)
                                continue;
                            float d = sqrt(d2);
                            float _s = 0.5 * (minDist - d) / d;
                            dxyz *= _s;
                            data->particles[i].position -= dxyz;
                            data->particles[id].position += dxyz;
                        }
                    }
                }
            }
        }
    }
}
// Iterate over particles
void FlipFluid::handleParticleCollisions()
{
    float r = particleRadius;

    float minX = h + r;
    float maxX = (fNumX - 1) * h - r;
    float minY = h + r;
    float maxY = (fNumY - 1) * h - r;
    float minZ = h + r;
    float maxZ = (fNumZ - 1) * h - r;

    for (int i = 0; i < numParticles; i++) {
        ParticleInfo& p = data->particles[i];

        // Clamp position and zero velocity on collision
        if (p.position.x < minX) {
            p.position.x = minX;
            p.velocity.x = 0.0f;
        } else if (p.position.x > maxX) {
            p.position.x = maxX;
            p.velocity.x = 0.0f;
        }

        if (p.position.y < minY) {
            p.position.y = minY;
            p.velocity.y = 0.0f;
        } else if (p.position.y > maxY) {
            p.position.y = maxY;
            p.velocity.y = 0.0f;
        }

        if (p.position.z < minZ) {
            p.position.z = minZ;
            p.velocity.z = 0.0f;
        } else if (p.position.z > maxZ) {
            p.position.z = maxZ;
            p.velocity.z = 0.0f;
        }
    }

}
// Iterate over fCells, must compute 2 sequential sums
void FlipFluid::updateParticleDensity()
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5 * h;

    for (int i = 0; i < fNumCells; i++)
        data->fCells[i].particleDensity = 0.0;

    for (int i = 0; i < numParticles; i++) {
        float x = clamp(data->particles[i].position.x, h, (fNumX - 1) * h);
        float y = clamp(data->particles[i].position.y, h, (fNumY - 1) * h);
        float z = clamp(data->particles[i].position.z, h, (fNumZ - 1) * h);

        int x0 = floor((x - h2) * h1);
        float tx = ((x - h2) - x0 * h) * h1;
        int x1 = min(x0 + 1, fNumX - 2);

        int y0 = floor((y - h2) * h1);
        float ty = ((y - h2) - y0 * h) * h1;
        int y1 = min(y0 + 1, fNumY - 2);

        int z0 = floor((z - h2) * h1);
        float tz = ((z - h2) - z0 * h) * h1;
        int z1 = min(z0 + 1, fNumZ - 2);

        float sx = 1.0 - tx;
        float sy = 1.0 - ty;
        float sz = 1.0 - tz;

        if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) data->fCells[(x0 * n + y0) * fNumZ + z0].particleDensity += sx * sy * sz;
        if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) data->fCells[(x1 * n + y0) * fNumZ + z0].particleDensity += tx * sy * sz;
        if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) data->fCells[(x1 * n + y1) * fNumZ + z0].particleDensity += tx * ty * sz;
        if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) data->fCells[(x0 * n + y1) * fNumZ + z0].particleDensity += sx * ty * sz;
        if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) data->fCells[(x0 * n + y0) * fNumZ + z1].particleDensity += sx * sy * tz;
        if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) data->fCells[(x1 * n + y0) * fNumZ + z1].particleDensity += tx * sy * tz;
        if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) data->fCells[(x1 * n + y1) * fNumZ + z1].particleDensity += tx * ty * tz;
        if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) data->fCells[(x0 * n + y1) * fNumZ + z1].particleDensity += sx * ty * tz;
    }

    if (particleRestDensity == 0.0) {
        float sum = 0.0;
        int numFluidCells = 0;

        for (int i = 0; i < fNumCells; i++) {
            if (data->fCells[i].cellType == FLUID_CELL) {
                sum += data->fCells[i].particleDensity;
                numFluidCells++;
            }
        }

        if (numFluidCells > 0)
            particleRestDensity = sum / numFluidCells;
    }
}
// Thrashing issue: reading and writing from and to fCells and particles
void FlipFluid::transferVelocities(const bool& toGrid, const float& flipRatio)
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5f * h;

    if (toGrid) {
        for (int i = 0; i < fNumCells; i++) {
            data->fCells[i].prevUVW = data->fCells[i].uvw;
        }

        for (int i = 0; i < fNumCells; i++) {
            data->fCells[i].duvw = vec3(0.0f);
            data->fCells[i].uvw = vec3(0.0f);
        }

        for (int i = 0; i < fNumCells; i++)
            data->fCells[i].cellType = data->fCells[i].s == 0.0 ? SOLID_CELL : AIR_CELL;

        for (int i = 0; i < numParticles; i++) {
            int xi = clamp(floor(data->particles[i].position.x * h1), 0.0f, float(fNumX - 1));
            int yi = clamp(floor(data->particles[i].position.y * h1), 0.0f, float(fNumY - 1));
            int zi = clamp(floor(data->particles[i].position.z * h1), 0.0f, float(fNumZ - 1));

            int cellNr = (xi * n + yi) * fNumZ + zi;
            if (data->fCells[cellNr].cellType == AIR_CELL)
                data->fCells[cellNr].cellType = FLUID_CELL;
        }
    }

    for (int component = 0; component < 3; component++) {
        float dx = (component == 0) ? 0.0 : h2;
        float dy = (component == 1) ? 0.0 : h2;
        float dz = (component == 2) ? 0.0 : h2;

        for (int i = 0; i < numParticles; i++) {
            float x = clamp(data->particles[i].position.x, h, (fNumX - 1) * h);
            float y = clamp(data->particles[i].position.y, h, (fNumY - 1) * h);
            float z = clamp(data->particles[i].position.z, h, (fNumZ - 1) * h);

            int x0 = min<float>(floor((x - dx) * h1), fNumX - 2);
            float tx = (x - dx - x0 * h) * h1;
            int x1 = min(x0 + 1, fNumX - 2);

            int y0 = min<float>(floor((y - dy) * h1), fNumY - 2);
            float ty = (y - dy - y0 * h) * h1;
            int y1 = min(y0 + 1, fNumY - 2);

            int z0 = min<float>(floor((z - dz) * h1), fNumZ - 2);
            float tz = (z - dz - z0 * h) * h1;
            int z1 = min(z0 + 1, fNumZ - 2);

            float sx = 1.0 - tx;
            float sy = 1.0 - ty;
            float sz = 1.0 - tz;

            float d0 = sx * sy * sz;
            float d1 = tx * sy * sz;
            float d2 = tx * ty * sz;
            float d3 = sx * ty * sz;
            float d4 = sx * sy * tz;
            float d5 = tx * sy * tz;
            float d6 = tx * ty * tz;
            float d7 = sx * ty * tz;

            int nr0 = (x0 * n + y0) * fNumZ + z0;
            int nr1 = (x1 * n + y0) * fNumZ + z0;
            int nr2 = (x1 * n + y1) * fNumZ + z0;
            int nr3 = (x0 * n + y1) * fNumZ + z0;
            int nr4 = (x0 * n + y0) * fNumZ + z1;
            int nr5 = (x1 * n + y0) * fNumZ + z1;
            int nr6 = (x1 * n + y1) * fNumZ + z1;
            int nr7 = (x0 * n + y1) * fNumZ + z1;

            if (toGrid) {
                float pv = data->particles[i].velocity.v[component];
                data->fCells[nr0].uvw[component] += pv * d0; data->fCells[nr0].duvw[component] += d0;
                data->fCells[nr1].uvw[component] += pv * d1; data->fCells[nr1].duvw[component] += d1;
                data->fCells[nr2].uvw[component] += pv * d2; data->fCells[nr2].duvw[component] += d2;
                data->fCells[nr3].uvw[component] += pv * d3; data->fCells[nr3].duvw[component] += d3;
                data->fCells[nr4].uvw[component] += pv * d4; data->fCells[nr4].duvw[component] += d4;
                data->fCells[nr5].uvw[component] += pv * d5; data->fCells[nr5].duvw[component] += d5;
                data->fCells[nr6].uvw[component] += pv * d6; data->fCells[nr6].duvw[component] += d6;
                data->fCells[nr7].uvw[component] += pv * d7; data->fCells[nr7].duvw[component] += d7;
            }
            else {
                int offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1);

                float valid0 = (data->fCells[nr0].cellType != AIR_CELL || data->fCells[nr0 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid1 = (data->fCells[nr1].cellType != AIR_CELL || data->fCells[nr1 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid2 = (data->fCells[nr2].cellType != AIR_CELL || data->fCells[nr2 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid3 = (data->fCells[nr3].cellType != AIR_CELL || data->fCells[nr3 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid4 = (data->fCells[nr4].cellType != AIR_CELL || data->fCells[nr4 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid5 = (data->fCells[nr5].cellType != AIR_CELL || data->fCells[nr5 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid6 = (data->fCells[nr6].cellType != AIR_CELL || data->fCells[nr6 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid7 = (data->fCells[nr7].cellType != AIR_CELL || data->fCells[nr7 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;

                float _v = data->particles[i].velocity.v[component];
                float _d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                           valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (_d > 0.0) {
                    float picV = (valid0 * d0 * data->fCells[nr0].uvw[component] + valid1 * d1 * data->fCells[nr1].uvw[component] +
                                  valid2 * d2 * data->fCells[nr2].uvw[component] + valid3 * d3 * data->fCells[nr3].uvw[component] +
                                  valid4 * d4 * data->fCells[nr4].uvw[component] + valid5 * d5 * data->fCells[nr5].uvw[component] +
                                  valid6 * d6 * data->fCells[nr6].uvw[component] + valid7 * d7 * data->fCells[nr7].uvw[component]) / _d;

                    float corr = (valid0 * d0 * (data->fCells[nr0].uvw[component] - data->fCells[nr0].prevUVW[component]) +
                                  valid1 * d1 * (data->fCells[nr1].uvw[component] - data->fCells[nr1].prevUVW[component]) +
                                  valid2 * d2 * (data->fCells[nr2].uvw[component] - data->fCells[nr2].prevUVW[component]) +
                                  valid3 * d3 * (data->fCells[nr3].uvw[component] - data->fCells[nr3].prevUVW[component]) +
                                  valid4 * d4 * (data->fCells[nr4].uvw[component] - data->fCells[nr4].prevUVW[component]) +
                                  valid5 * d5 * (data->fCells[nr5].uvw[component] - data->fCells[nr5].prevUVW[component]) +
                                  valid6 * d6 * (data->fCells[nr6].uvw[component] - data->fCells[nr6].prevUVW[component]) +
                                  valid7 * d7 * (data->fCells[nr7].uvw[component] - data->fCells[nr7].prevUVW[component])) / _d;

                    float flipV = _v + corr;

                    data->particles[i].velocity.v[component] = (1.0 - flipRatio) * picV + flipRatio * flipV;
                }
            }
        }

        if (toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (data->fCells[i].duvw[component] > 0.0)
                    data->fCells[i].uvw[component] /= data->fCells[i].duvw[component];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int cellIndex = (i * n + j) * fNumZ + k;
                        int solid = data->fCells[cellIndex].cellType == SOLID_CELL;

                        if (component == 0) {
                            if (solid || (i > 0 && data->fCells[((i - 1) * n + j) * fNumZ + k].cellType == SOLID_CELL))
                                data->fCells[cellIndex].uvw.x = 0.0;
                        }
                        else if (component == 1) {
                            if (solid || (j > 0 && data->fCells[(i * n + (j - 1)) * fNumZ + k].cellType == SOLID_CELL))
                                data->fCells[cellIndex].uvw.y = 0.0;
                        }
                        else { // component == 2
                            if (solid || (k > 0 && data->fCells[(i * n + j) * fNumZ + (k - 1)].cellType == SOLID_CELL))
                                data->fCells[cellIndex].uvw.z = 0.0;
                        }
                    }
                }
            }
        }
    }
}
// Iterate over fCells
void FlipFluid::solveIncompressibility(const int& numIters, const float& dt, const float& overRelaxation, const bool& compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        data->fCells[i].p = 0.0;
        data->fCells[i].prevUVW = data->fCells[i].uvw;
    }

    int n = fNumY;
    float cp = density * h / dt;

    for (int iter = 0; iter < numIters; iter++) {
        for (int i = 1; i < fNumX - 1; i++) {
            for (int j = 1; j < fNumY - 1; j++) {
                for (int k = 1; k < fNumZ - 1; k++) {

                    if (data->fCells[(i * n + j) * fNumZ + k].cellType != FLUID_CELL)
                        continue;

                    int center = (i * n + j) * fNumZ + k;
                    int left = ((i - 1) * n + j) * fNumZ + k;
                    int right = ((i + 1) * n + j) * fNumZ + k;
                    int bottom = (i * n + j - 1) * fNumZ + k;
                    int top = (i * n + j + 1) * fNumZ + k;
                    int back = (i * n + j) * fNumZ + k - 1;
                    int front = (i * n + j) * fNumZ + k + 1;

                    float sx0 = data->fCells[left].s;
                    float sx1 = data->fCells[right].s;
                    float sy0 = data->fCells[bottom].s;
                    float sy1 = data->fCells[top].s;
                    float sz0 = data->fCells[back].s;
                    float sz1 = data->fCells[front].s;
                    float _s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (_s == 0.0)
                        continue;

                    float div = data->fCells[right].uvw.x - data->fCells[center].uvw.x + data->fCells[top].uvw.y - data->fCells[center].uvw.y + data->fCells[front].uvw.z - data->fCells[center].uvw.z;

                    if (particleRestDensity > 0.0 && compensateDrift) {
                        float _k = 1.0;
                        float compression = data->fCells[(i * n + j) * fNumZ + k].particleDensity - particleRestDensity;
                        if (compression > 0.0)
                            div -= _k * compression;
                    }

                    float _p = -div / _s * overRelaxation;
                    data->fCells[center].p += cp * _p;

                    data->fCells[center].uvw.x -= sx0 * _p;
                    data->fCells[right].uvw.x += sx1 * _p;
                    data->fCells[center].uvw.y -= sy0 * _p;
                    data->fCells[top].uvw.y += sy1 * _p;
                    data->fCells[center].uvw.z -= sz0 * _p;
                    data->fCells[front].uvw.z += sz1 * _p;
                }
            }
        }
    }
}

void FlipFluid::simulate(const vec3 &gravity)
{
    int numSubSteps = 1;
    float sdt = dt / numSubSteps;

    for (int step = 0; step < numSubSteps; step++) {
        integrateParticles(sdt, gravity);
        if (separateParticles)
            pushParticlesApart(numParticleIters);
        handleParticleCollisions();
        transferVelocities(true, flipRatio);
        updateParticleDensity();
        solveIncompressibility(numPressureIters, sdt, overRelaxation, compensateDrift);
        transferVelocities(false, flipRatio);
    }
}