//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "FlipFluid.h"

FlipFluid::FlipFluid(const float& _density, const float& _width, const float& _height, const float& _depth, const float& _spacing, const float& _particleRadius, const int& _maxParticles) {

    // fluid properties

    density = _density;
    fNumX = floor(_width / _spacing) + 1;
    fNumY = floor(_height / _spacing) + 1;
    fNumZ = floor(_depth / _spacing) + 1;
    h = max(max(_width / fNumX, _height / fNumY), _depth / fNumZ);
    fInvSpacing = 1.0 / h;
    fNumCells = fNumX * fNumY * fNumZ;

    u = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the x-direction
    v = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the y-direction
    w = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the z-direction
    du = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the x-direction
    dv = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the y-direction
    dw = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the z-direction
    prevU = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the x-direction
    prevV = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the y-direction
    prevW = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the z-direction
    p = (float*)calloc(fNumCells, sizeof(float));         // Initialize pressure
    s = (float*)calloc(fNumCells, sizeof(float));         // Initialize source term
    cellType = (int*)calloc(fNumCells, sizeof(int));        // Initialize cell type (fluid, air, solid)

    // particle properties

    maxParticles = _maxParticles;

    particlePos = (vec3*)calloc(maxParticles, sizeof(vec3));   // Initialize particle positions (x, y, z)
    particleColor = (vec3*)calloc(maxParticles, sizeof(vec3)); // Initialize particle color (for visualization)

    particleVel = (vec3*)calloc(maxParticles, sizeof(vec3));      // Initialize particle velocity (x, y, z)
    particleDensity = (float*)calloc(fNumCells, sizeof(float));         // Initialize particle density
    particleRestDensity = 0.0;                                            // Set the rest density of the particles

    particleRadius = _particleRadius;
    pInvSpacing = 1.0 / (2.2 * _particleRadius);
    pNumX = floor(_width * pInvSpacing) + 1;
    pNumY = floor(_height * pInvSpacing) + 1;
    pNumZ = floor(_depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;

    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(_maxParticles, sizeof(int));           // Initialize the particle ids

    numParticles = 0;                                                     // Set the initial number of particles to zero
}

void FlipFluid::integrateParticles(const float& _dt, const vec3& _gravity)
{
    for (int i = 0; i < numParticles; i++) {
        particleVel[i] += _dt * _gravity;
        particlePos[i] += particleVel[i] * _dt;
    }
}

void FlipFluid::pushParticlesApart(const float& _numIters)
{
    float colorDiffusionCoeff = 0.001;

    // count particles per cell

    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

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

    // fill particles into cells

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        int xi = clamp(floor(x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }

    // push particles apart

    float minDist = 2.0 * particleRadius;
    float minDist2 = minDist * minDist;

    for (int iter = 0; iter < _numIters; iter++) {

        for (int i = 0; i < numParticles; i++) {
            float px = particlePos[i].x;
            float py = particlePos[i].y;
            float pz = particlePos[i].z;

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
                            float qx = particlePos[id].x;
                            float qy = particlePos[id].y;
                            float qz = particlePos[id].z;

                            float dx = qx - px;
                            float dy = qy - py;
                            float dz = qz - pz;
                            float d2 = dx * dx + dy * dy + dz * dz;
                            if (d2 > minDist2 || d2 == 0.0)
                                continue;
                            float d = sqrt(d2);
                            float s = 0.5 * (minDist - d) / d;
                            dx *= s;
                            dy *= s;
                            dz *= s;
                            particlePos[i].x -= dx;
                            particlePos[i].y -= dy;
                            particlePos[i].z -= dz;
                            particlePos[id].x += dx;
                            particlePos[id].y += dy;
                            particlePos[id] += dz;

                            // diffuse colors

                            for (int k = 0; k < 3; k++) {
                                float color0 = particleColor[i].v[k];
                                float color1 = particleColor[id].v[k];
                                float color = (color0 + color1) * 0.5;
                                particleColor[i].v[k] = color0 + (color - color0) * colorDiffusionCoeff;
                                particleColor[id].v[k] = color1 + (color - color1) * colorDiffusionCoeff;
                            }
                        }
                    }
                }
            }
        }
    }
}

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
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        // wall collisions

        if (x < minX) {
            x = minX;
            particleVel[i].x = 0.0;
        }
        if (x > maxX) {
            x = maxX;
            particleVel[i].x = 0.0;
        }
        if (y < minY) {
            y = minY;
            particleVel[i].y = 0.0;
        }
        if (y > maxY) {
            y = maxY;
            particleVel[i].y = 0.0;
        }
        if (z < minZ) {
            z = minZ;
            particleVel[i].z = 0.0;
        }
        if (z > maxZ) {
            z = maxZ;
            particleVel[i].z = 0.0;
        }
        particlePos[i].x = x;
        particlePos[i].y = y;
        particlePos[i].z = z;
    }
}

void FlipFluid::updateParticleDensity()
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5 * h;

    float* d = particleDensity;

    for (int i = 0; i < fNumCells; i++)
        d[i] = 0.0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        x = clamp(x, h, (fNumX - 1) * h);
        y = clamp(y, h, (fNumY - 1) * h);
        z = clamp(z, h, (fNumZ - 1) * h);

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

        if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z0] += sx * sy * sz;
        if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z0] += tx * sy * sz;
        if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z0] += tx * ty * sz;
        if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z0] += sx * ty * sz;
        if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z1] += sx * sy * tz;
        if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z1] += tx * sy * tz;
        if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z1] += tx * ty * tz;
        if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z1] += sx * ty * tz;
    }

    if (particleRestDensity == 0.0) {
        float sum = 0.0;
        int numFluidCells = 0;

        for (int i = 0; i < fNumCells; i++) {
            if (cellType[i] == FLUID_CELL) {
                sum += d[i];
                numFluidCells++;
            }
        }

        if (numFluidCells > 0)
            particleRestDensity = sum / numFluidCells;
    }
}

void FlipFluid::transferVelocities(const bool& _toGrid, const float& _flipRatio)
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5f * h;

    if (_toGrid) {
        for (int i = 0; i < fNumCells; i++) {
            prevU[i] = u[i];
            prevV[i] = v[i];
            prevW[i] = w[i];
        }

        for (int i = 0; i < fNumCells; i++) {
            du[i] = 0.0;
            dv[i] = 0.0;
            dw[i] = 0.0;
            u[i] = 0.0;
            v[i] = 0.0;
            w[i] = 0.0;
        }

        for (int i = 0; i < fNumCells; i++)
            cellType[i] = s[i] == 0.0 ? SOLID_CELL : AIR_CELL;

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            int xi = clamp(floor(x * h1), 0.0f, float(fNumX - 1));
            int yi = clamp(floor(y * h1), 0.0f, float(fNumY - 1));
            int zi = clamp(floor(z * h1), 0.0f, float(fNumZ - 1));

            int cellNr = xi * (n * fNumZ) + yi * fNumZ + zi;
            if (cellType[cellNr] == AIR_CELL)
                cellType[cellNr] = FLUID_CELL;
        }
    }

    for (int component = 0; component < 3; component++) {
        float dx = (component == 0) ? 0.0 : h2;
        float dy = (component == 1) ? 0.0 : h2;
        float dz = (component == 2) ? 0.0 : h2;

        float* f = (component == 0) ? u : ((component == 1) ? v : w);
        float* prevF = (component == 0) ? prevU : ((component == 1) ? prevV : prevW);
        float* d = (component == 0) ? du : ((component == 1) ? dv : dw);

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            x = clamp(x, h, (fNumX - 1) * h);
            y = clamp(y, h, (fNumY - 1) * h);
            z = clamp(z, h, (fNumZ - 1) * h);

            int x0 = min<float>(floor((x - dx) * h1), fNumX - 2);
            float tx = ((x - dx) - x0 * h) * h1;
            int x1 = min(x0 + 1, fNumX - 2);

            int y0 = min<float>(floor((y - dy) * h1), fNumY - 2);
            float ty = ((y - dy) - y0 * h) * h1;
            int y1 = min(y0 + 1, fNumY - 2);

            int z0 = min<float>(floor((z - dz) * h1), fNumZ - 2);
            float tz = ((z - dz) - z0 * h) * h1;
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

            int nr0 = x0 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr1 = x1 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr2 = x1 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr3 = x0 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr4 = x0 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr5 = x1 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr6 = x1 * (n * fNumZ) + y1 * fNumZ + z1;
            int nr7 = x0 * (n * fNumZ) + y1 * fNumZ + z1;

            if (_toGrid) {
                float pv = particleVel[i].v[component];
                f[nr0] += pv * d0; d[nr0] += d0;
                f[nr1] += pv * d1; d[nr1] += d1;
                f[nr2] += pv * d2; d[nr2] += d2;
                f[nr3] += pv * d3; d[nr3] += d3;
                f[nr4] += pv * d4; d[nr4] += d4;
                f[nr5] += pv * d5; d[nr5] += d5;
                f[nr6] += pv * d6; d[nr6] += d6;
                f[nr7] += pv * d7; d[nr7] += d7;
            }
            else {
                int offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1);

                float valid0 = (cellType[nr0] != AIR_CELL || cellType[nr0 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid1 = (cellType[nr1] != AIR_CELL || cellType[nr1 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid2 = (cellType[nr2] != AIR_CELL || cellType[nr2 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid3 = (cellType[nr3] != AIR_CELL || cellType[nr3 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid4 = (cellType[nr4] != AIR_CELL || cellType[nr4 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid5 = (cellType[nr5] != AIR_CELL || cellType[nr5 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid6 = (cellType[nr6] != AIR_CELL || cellType[nr6 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid7 = (cellType[nr7] != AIR_CELL || cellType[nr7 - offset] != AIR_CELL) ? 1.0 : 0.0;

                float v = particleVel[i].v[component];
                float d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                          valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (d > 0.0) {
                    float picV = (valid0 * d0 * f[nr0] + valid1 * d1 * f[nr1] + valid2 * d2 * f[nr2] + valid3 * d3 * f[nr3] +
                                  valid4 * d4 * f[nr4] + valid5 * d5 * f[nr5] + valid6 * d6 * f[nr6] + valid7 * d7 * f[nr7]) / d;
                    float corr = (valid0 * d0 * (f[nr0] - prevF[nr0]) + valid1 * d1 * (f[nr1] - prevF[nr1]) +
                                  valid2 * d2 * (f[nr2] - prevF[nr2]) + valid3 * d3 * (f[nr3] - prevF[nr3]) +
                                  valid4 * d4 * (f[nr4] - prevF[nr4]) + valid5 * d5 * (f[nr5] - prevF[nr5]) +
                                  valid6 * d6 * (f[nr6] - prevF[nr6]) + valid7 * d7 * (f[nr7] - prevF[nr7])) / d;
                    float flipV = v + corr;

                    particleVel[i].v[component] = (1.0 - _flipRatio) * picV + _flipRatio * flipV;
                }
            }
        }

        if (_toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (d[i] > 0.0)
                    f[i] /= d[i];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int solid = cellType[i * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL;
                        if (solid || (i > 0 && cellType[(i - 1) * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL))
                            u[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (j > 0 && cellType[i * (n * fNumZ) + (j - 1) * fNumZ + k] == SOLID_CELL))
                            v[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (k > 0 && cellType[i * (n * fNumZ) + j * fNumZ + (k - 1)] == SOLID_CELL))
                            w[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                    }
                }
            }
        }
    }
}

void FlipFluid::solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        p[i] = 0.0;
        prevU[i] = u[i];
        prevV[i] = v[i];
        prevW[i] = w[i];
    }

    int n = fNumY;
    float cp = density * h / _dt;

    for (int iter = 0; iter < _numIters; iter++) {
        for (int i = 1; i < fNumX - 1; i++) {
            for (int j = 1; j < fNumY - 1; j++) {
                for (int k = 1; k < fNumZ - 1; k++) {

                    if (cellType[i * (n * fNumZ) + j * fNumZ + k] != FLUID_CELL)
                        continue;

                    int center = i * (n * fNumZ) + j * fNumZ + k;
                    int left = (i - 1) * (n * fNumZ) + j * fNumZ + k;
                    int right = (i + 1) * (n * fNumZ) + j * fNumZ + k;
                    int bottom = i * (n * fNumZ) + (j - 1) * fNumZ + k;
                    int top = i * (n * fNumZ) + (j + 1) * fNumZ + k;
                    int back = i * (n * fNumZ) + j * fNumZ + (k - 1);
                    int front = i * (n * fNumZ) + j * fNumZ + (k + 1);

                    float sx0 = s[left];
                    float sx1 = s[right];
                    float sy0 = s[bottom];
                    float sy1 = s[top];
                    float sz0 = s[back];
                    float sz1 = s[front];
                    float s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (s == 0.0)
                        continue;

                    float div = u[right] - u[center] +
                                v[top] - v[center] +
                                w[front] - w[center];

                    if (particleRestDensity > 0.0 && _compensateDrift) {
                        float _k = 1.0;
                        float compression = particleDensity[i * (n * fNumZ) + j * fNumZ + k] - particleRestDensity;
                        if (compression > 0.0)
                            div = div - _k * compression;
                    }

                    float p = -div / s;
                    p *= _overRelaxation;
                    this->p[center] += cp * p;

                    u[center] -= sx0 * p;
                    u[right] += sx1 * p;
                    v[center] -= sy0 * p;
                    v[top] += sy1 * p;
                    w[center] -= sz0 * p;
                    w[front] += sz1 * p;
                }
            }
        }
    }
}

void FlipFluid::updateParticleColors()
{
    float h1 = fInvSpacing;

    for (int i = 0; i < numParticles; i++) {
        float s = 0.01;

        particleColor[i].r = clamp(particleColor[i].r - s, 0.0f, 1.0f);
        particleColor[i].g = clamp(particleColor[i].g - s, 0.0f, 1.0f);
        particleColor[i].b = clamp(particleColor[i].b + s, 0.0f, 1.0f);

        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;
        int xi = clamp(floor(x * h1), 1.0f, float(fNumX - 1));
        int yi = clamp(floor(y * h1), 1.0f, float(fNumY - 1));
        int zi = clamp(floor(z * h1), 1.0f, float(fNumZ - 1));
        int cellNr = xi * fNumY * fNumZ + yi * fNumZ + zi;

        float d0 = particleRestDensity;

        if (d0 > 0.0) {
            float relDensity = particleDensity[cellNr] / d0;
            if (relDensity < 0.7) {
                float s = 0.8;
                particleColor[i].r = s;
                particleColor[i].g = s;
                particleColor[i].b = 1.0;
            }
        }
    }
}

void FlipFluid::simulate(const float& _dt, const vec3& _gravity, const float& _flipRatio, const int& _numPressureIters, const int& _numParticleIters, const float& _overRelaxation, const bool& _compensateDrift, const bool& _separateParticles)
{
    int numSubSteps = 1;
    float sdt = _dt / numSubSteps;

    for (int step = 0; step < numSubSteps; step++) {
        this->integrateParticles(sdt, _gravity);
        if (_separateParticles)
            this->pushParticlesApart(_numParticleIters);
        this->handleParticleCollisions();
        this->transferVelocities(true, _flipRatio);
        this->updateParticleDensity();
        this->solveIncompressibility(_numPressureIters, sdt, _overRelaxation, _compensateDrift);
        this->transferVelocities(false, _flipRatio);
    }

    this->updateParticleColors();
}
