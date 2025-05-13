//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "FlipFluid.h"

FlipFluid::FlipFluid() {

    const int width = 1000;

    const int height = 1000;

    float simHeight;

    float cScale;

    float simWidth;

    float simDepth;

    simHeight = 3.0;
    cScale = height / simHeight;
    simWidth = width / cScale;
    simDepth = simHeight;  // Assuming the depth is the same as the height

    int res = 15;

    float tankHeight = 1.0f * simHeight;
    float tankWidth = 1.0f * simWidth;
    float tankDepth = 1.0f * simDepth;
    float spacing = tankHeight / res;
    density = 1000.0f;

    float relWaterHeight = 1.0f;
    float relWaterWidth = 0.6f;
    float relWaterDepth = 1.0f;

    // compute number of stars

    particleRadius = 0.3 * spacing;    // particle radius w.r.t. cell size
    float dx = 2.0 * particleRadius;
    float dy = sqrt(3.0) / 2.0 * dx;
    float dz = 2.0 * particleRadius;

    int numX = floor((relWaterWidth * tankWidth - 2.0 * spacing - 2.0 * particleRadius) / dx);
    int numY = floor((relWaterHeight * tankHeight - 2.0 * spacing - 2.0 * particleRadius) / dy);
    int numZ = floor((relWaterDepth * tankDepth - 2.0 * spacing - 2.0 * particleRadius) / dz);
    maxParticles = numX * numY * numZ;

    // fluid properties

    fNumX = floor(tankWidth / spacing) + 1;
    fNumY = floor(tankHeight / spacing) + 1;
    fNumZ = floor(tankDepth / spacing) + 1;
    h = max(max(tankWidth / fNumX, tankHeight / fNumY), tankDepth / fNumZ);
    fInvSpacing = 1.0 / h;
    fNumCells = fNumX * fNumY * fNumZ;

    // Initialize velocity fields as vector fields instead of separate components
    fCells = (fCell*)calloc(fNumCells, sizeof(fCell)); // Initialize fluid grid cells velocity properties

    // particle properties

    particles = (ParticleInfo*)calloc(maxParticles, sizeof(ParticleInfo)); // Initialize particle data
    particleRestDensity = 0.0;                                            // Set the rest density of the stars

    pInvSpacing = 1.0 / (2.2 * particleRadius);
    pNumX = floor(tankWidth * pInvSpacing) + 1;
    pNumY = floor(tankHeight * pInvSpacing) + 1;
    pNumZ = floor(tankDepth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;

    pCells = (pCell*)calloc(pNumCells + 1, sizeof(pCell)); // Initialize per-cell information

    numParticles = numX * numY * numZ;
    int index = 0;
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            for (int k = 0; k < numZ; k++) {
                particles[index++].position = vec3(
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
                fCells[(i * fNumY + j) * fNumZ + k].s = cellDensity;
            }
        }
    }
}

void FlipFluid::integrateParticles(const float &_dt, const vec3 &_gravity)
{
    for (int i = 0; i < numParticles; i++) {
        particles[i].velocity += _dt * _gravity;
        particles[i].position += particles[i].velocity * _dt;
    }
}

void FlipFluid::pushParticlesApart(const float& _numIters)
{
    float colorDiffusionCoeff = 0.001;

    // particleCount stars per cell

    for (int i = 0; i < pNumCells; i++)
        pCells[i].numCellParticles = 0;

    for (int i = 0; i < numParticles; i++) {
        int xi = clamp(floor(particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
        pCells[cellNr].numCellParticles++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += pCells[i].numCellParticles;
        pCells[i].firstCellParticle = first;
    }
    pCells[pNumCells].firstCellParticle = first; // guard

    // fill stars into cells

    for (int i = 0; i < numParticles; i++) {
        int xi = clamp(floor(particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
        pCells[cellNr].firstCellParticle--;
        particles[pCells[cellNr].firstCellParticle].cellParticleId = i;
    }

    // push stars apart

    float minDist = 2.0 * particleRadius;
    float minDist2 = minDist * minDist;

    for (int iter = 0; iter < _numIters; iter++) {

        for (int i = 0; i < numParticles; i++) {
            float px = particles[i].position.x;
            float py = particles[i].position.y;
            float pz = particles[i].position.z;

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
                        int cellNr = (xi * pNumY + yi) * pNumZ + zi;
                        first = pCells[cellNr].firstCellParticle;
                        int last = pCells[cellNr + 1].firstCellParticle;
                        for (int j = first; j < last; j++) {
                            int id = particles[j].cellParticleId;
                            if (id == i)
                                continue;
                            float dx = particles[id].position.x - px;
                            float dy = particles[id].position.y - py;
                            float dz = particles[id].position.z - pz;
                            float d2 = dx * dx + dy * dy + dz * dz;
                            if (d2 > minDist2 || d2 == 0.0)
                                continue;
                            float d = sqrt(d2);
                            float _s = 0.5 * (minDist - d) / d;
                            dx *= _s;
                            dy *= _s;
                            dz *= _s;
                            particles[i].position.x -= dx;
                            particles[i].position.y -= dy;
                            particles[i].position.z -= dz;
                            particles[id].position.x += dx;
                            particles[id].position.y += dy;
                            particles[id].position.z += dz;
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
        float x = particles[i].position.x;
        float y = particles[i].position.y;
        float z = particles[i].position.z;

        // wall collisions

        if (x < minX) {
            x = minX;
            particles[i].velocity.x = 0.0;
        }
        if (x > maxX) {
            x = maxX;
            particles[i].velocity.x = 0.0;
        }
        if (y < minY) {
            y = minY;
            particles[i].velocity.y = 0.0;
        }
        if (y > maxY) {
            y = maxY;
            particles[i].velocity.y = 0.0;
        }
        if (z < minZ) {
            z = minZ;
            particles[i].velocity.z = 0.0;
        }
        if (z > maxZ) {
            z = maxZ;
            particles[i].velocity.z = 0.0;
        }
        particles[i].position.x = x;
        particles[i].position.y = y;
        particles[i].position.z = z;
    }

}

void FlipFluid::updateParticleDensity()
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5 * h;

    for (int i = 0; i < fNumCells; i++)
        fCells[i].particleDensity = 0.0;

    for (int i = 0; i < numParticles; i++) {
        float x = clamp(particles[i].position.x, h, (fNumX - 1) * h);
        float y = clamp(particles[i].position.y, h, (fNumY - 1) * h);
        float z = clamp(particles[i].position.z, h, (fNumZ - 1) * h);

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

        if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) fCells[(x0 * n + y0) * fNumZ + z0].particleDensity += sx * sy * sz;
        if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) fCells[(x1 * n + y0) * fNumZ + z0].particleDensity += tx * sy * sz;
        if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) fCells[(x1 * n + y1) * fNumZ + z0].particleDensity += tx * ty * sz;
        if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) fCells[(x0 * n + y1) * fNumZ + z0].particleDensity += sx * ty * sz;
        if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) fCells[(x0 * n + y0) * fNumZ + z1].particleDensity += sx * sy * tz;
        if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) fCells[(x1 * n + y0) * fNumZ + z1].particleDensity += tx * sy * tz;
        if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) fCells[(x1 * n + y1) * fNumZ + z1].particleDensity += tx * ty * tz;
        if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) fCells[(x0 * n + y1) * fNumZ + z1].particleDensity += sx * ty * tz;
    }

    if (particleRestDensity == 0.0) {
        float sum = 0.0;
        int numFluidCells = 0;

        for (int i = 0; i < fNumCells; i++) {
            if (fCells[i].cellType == FLUID_CELL) {
                sum += fCells[i].particleDensity;
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
            fCells[i].prevUVW = fCells[i].uvw;
        }

        for (int i = 0; i < fNumCells; i++) {
            fCells[i].duvw = vec3(0.0f);
            fCells[i].uvw = vec3(0.0f);
        }

        for (int i = 0; i < fNumCells; i++)
            fCells[i].cellType = fCells[i].s == 0.0 ? SOLID_CELL : AIR_CELL;

        for (int i = 0; i < numParticles; i++) {
            int xi = clamp(floor(particles[i].position.x * h1), 0.0f, float(fNumX - 1));
            int yi = clamp(floor(particles[i].position.y * h1), 0.0f, float(fNumY - 1));
            int zi = clamp(floor(particles[i].position.z * h1), 0.0f, float(fNumZ - 1));

            int cellNr = (xi * n + yi) * fNumZ + zi;
            if (fCells[cellNr].cellType == AIR_CELL)
                fCells[cellNr].cellType = FLUID_CELL;
        }
    }

    for (int component = 0; component < 3; component++) {
        float dx = (component == 0) ? 0.0 : h2;
        float dy = (component == 1) ? 0.0 : h2;
        float dz = (component == 2) ? 0.0 : h2;

        for (int i = 0; i < numParticles; i++) {
            float x = particles[i].position.x;
            float y = particles[i].position.y;
            float z = particles[i].position.z;

            x = clamp(x, h, (fNumX - 1) * h);
            y = clamp(y, h, (fNumY - 1) * h);
            z = clamp(z, h, (fNumZ - 1) * h);

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

            if (_toGrid) {
                float pv = particles[i].velocity.v[component];
                fCells[nr0].uvw[component] += pv * d0; fCells[nr0].duvw[component] += d0;
                fCells[nr1].uvw[component] += pv * d1; fCells[nr1].duvw[component] += d1;
                fCells[nr2].uvw[component] += pv * d2; fCells[nr2].duvw[component] += d2;
                fCells[nr3].uvw[component] += pv * d3; fCells[nr3].duvw[component] += d3;
                fCells[nr4].uvw[component] += pv * d4; fCells[nr4].duvw[component] += d4;
                fCells[nr5].uvw[component] += pv * d5; fCells[nr5].duvw[component] += d5;
                fCells[nr6].uvw[component] += pv * d6; fCells[nr6].duvw[component] += d6;
                fCells[nr7].uvw[component] += pv * d7; fCells[nr7].duvw[component] += d7;
            }
            else {
                int offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1);

                float valid0 = (fCells[nr0].cellType != AIR_CELL || fCells[nr0 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid1 = (fCells[nr1].cellType != AIR_CELL || fCells[nr1 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid2 = (fCells[nr2].cellType != AIR_CELL || fCells[nr2 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid3 = (fCells[nr3].cellType != AIR_CELL || fCells[nr3 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid4 = (fCells[nr4].cellType != AIR_CELL || fCells[nr4 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid5 = (fCells[nr5].cellType != AIR_CELL || fCells[nr5 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid6 = (fCells[nr6].cellType != AIR_CELL || fCells[nr6 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;
                float valid7 = (fCells[nr7].cellType != AIR_CELL || fCells[nr7 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;

                float _v = particles[i].velocity.v[component];
                float _d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                           valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (_d > 0.0) {
                    float picV = (valid0 * d0 * fCells[nr0].uvw[component] + valid1 * d1 * fCells[nr1].uvw[component] +
                                  valid2 * d2 * fCells[nr2].uvw[component] + valid3 * d3 * fCells[nr3].uvw[component] +
                                  valid4 * d4 * fCells[nr4].uvw[component] + valid5 * d5 * fCells[nr5].uvw[component] +
                                  valid6 * d6 * fCells[nr6].uvw[component] + valid7 * d7 * fCells[nr7].uvw[component]) / _d;

                    float corr = (valid0 * d0 * (fCells[nr0].uvw[component] - fCells[nr0].prevUVW[component]) +
                                  valid1 * d1 * (fCells[nr1].uvw[component] - fCells[nr1].prevUVW[component]) +
                                  valid2 * d2 * (fCells[nr2].uvw[component] - fCells[nr2].prevUVW[component]) +
                                  valid3 * d3 * (fCells[nr3].uvw[component] - fCells[nr3].prevUVW[component]) +
                                  valid4 * d4 * (fCells[nr4].uvw[component] - fCells[nr4].prevUVW[component]) +
                                  valid5 * d5 * (fCells[nr5].uvw[component] - fCells[nr5].prevUVW[component]) +
                                  valid6 * d6 * (fCells[nr6].uvw[component] - fCells[nr6].prevUVW[component]) +
                                  valid7 * d7 * (fCells[nr7].uvw[component] - fCells[nr7].prevUVW[component])) / _d;

                    float flipV = _v + corr;

                    particles[i].velocity.v[component] = (1.0 - _flipRatio) * picV + _flipRatio * flipV;
                }
            }
        }

        if (_toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (fCells[i].duvw[component] > 0.0)
                    fCells[i].uvw[component] /= fCells[i].duvw[component];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int cellIndex = (i * n + j) * fNumZ + k;
                        int solid = fCells[cellIndex].cellType == SOLID_CELL;

                        if (component == 0) {
                            if (solid || (i > 0 && fCells[((i - 1) * n + j) * fNumZ + k].cellType == SOLID_CELL))
                                fCells[cellIndex].uvw.x = 0.0;
                        }
                        else if (component == 1) {
                            if (solid || (j > 0 && fCells[(i * n + (j - 1)) * fNumZ + k].cellType == SOLID_CELL))
                                fCells[cellIndex].uvw.y = 0.0;
                        }
                        else { // component == 2
                            if (solid || (k > 0 && fCells[(i * n + j) * fNumZ + (k - 1)].cellType == SOLID_CELL))
                                fCells[cellIndex].uvw.z = 0.0;
                        }
                    }
                }
            }
        }
    }
}

void FlipFluid::solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        fCells[i].p = 0.0;
        fCells[i].prevUVW = fCells[i].uvw;
    }

    int n = fNumY;
    float cp = density * h / _dt;

    for (int iter = 0; iter < _numIters; iter++) {
        for (int i = 1; i < fNumX - 1; i++) {
            for (int j = 1; j < fNumY - 1; j++) {
                for (int k = 1; k < fNumZ - 1; k++) {

                    if (fCells[(i * n + j) * fNumZ + k].cellType != FLUID_CELL)
                        continue;

                    int center = (i * n + j) * fNumZ + k;
                    int left = ((i - 1) * n + j) * fNumZ + k;
                    int right = ((i + 1) * n + j) * fNumZ + k;
                    int bottom = (i * n + (j - 1)) * fNumZ + k;
                    int top = (i * n + (j + 1)) * fNumZ + k;
                    int back = (i * n + j) * fNumZ + (k - 1);
                    int front = (i * n + j) * fNumZ + (k + 1);

                    float sx0 = fCells[left].s;
                    float sx1 = fCells[right].s;
                    float sy0 = fCells[bottom].s;
                    float sy1 = fCells[top].s;
                    float sz0 = fCells[back].s;
                    float sz1 = fCells[front].s;
                    float _s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (_s == 0.0)
                        continue;

                    float div = fCells[right].uvw.x - fCells[center].uvw.x + fCells[top].uvw.y - fCells[center].uvw.y + fCells[front].uvw.z - fCells[center].uvw.z;

                    if (particleRestDensity > 0.0 && _compensateDrift) {
                        float _k = 1.0;
                        float compression = fCells[(i * n + j) * fNumZ + k].particleDensity - particleRestDensity;
                        if (compression > 0.0)
                            div = div - _k * compression;
                    }

                    float _p = -div / _s * _overRelaxation;
                    fCells[center].p += cp * _p;

                    fCells[center].uvw.x -= sx0 * _p;
                    fCells[right].uvw.x += sx1 * _p;
                    fCells[center].uvw.y -= sy0 * _p;
                    fCells[top].uvw.y += sy1 * _p;
                    fCells[center].uvw.z -= sz0 * _p;
                    fCells[front].uvw.z += sz1 * _p;
                }
            }
        }
    }
}

void FlipFluid::simulate(const vec3 &_gravity)
{
    int numSubSteps = 1;
    float sdt = dt / numSubSteps;

    for (int step = 0; step < numSubSteps; step++) {
        integrateParticles(sdt, _gravity);
        if (separateParticles)
            pushParticlesApart(numParticleIters);
        handleParticleCollisions();
        transferVelocities(true, flipRatio);
        updateParticleDensity();
        solveIncompressibility(numPressureIters, sdt, overRelaxation, compensateDrift);
        transferVelocities(false, flipRatio);
    }
}