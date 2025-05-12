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
    uvw = (vec3*)calloc(fNumCells, sizeof(vec3));         // Initialize fluid velocity as vector field
    duvw = (vec3*)calloc(fNumCells, sizeof(vec3));        // Initialize the change in fluid velocity as vector field
    prevUVW = (vec3*)calloc(fNumCells, sizeof(vec3));     // Initialize the previous fluid velocity as vector field

    p = (float*)calloc(fNumCells, sizeof(float));         // Initialize pressure
    s = (float*)calloc(fNumCells, sizeof(float));         // Initialize source term
    cellType = (int*)calloc(fNumCells, sizeof(int));        // Initialize cell type (fluid, air, solid)

    // particle properties

    particlePos = (vec3*)calloc(maxParticles, sizeof(vec3));   // Initialize particle positions (x, y, z)
    particleColor = (vec3*)calloc(maxParticles, sizeof(vec3)); // Initialize particle color (for visualization)

    particleVel = (vec3*)calloc(maxParticles, sizeof(vec3));      // Initialize particle velocity (x, y, z)
    particleDensity = (float*)calloc(fNumCells, sizeof(float));         // Initialize particle density
    particleRestDensity = 0.0;                                            // Set the rest density of the stars

    pInvSpacing = 1.0 / (2.2 * particleRadius);
    pNumX = floor(tankWidth * pInvSpacing) + 1;
    pNumY = floor(tankHeight * pInvSpacing) + 1;
    pNumZ = floor(tankDepth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;

    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of stars in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(maxParticles, sizeof(int));           // Initialize the particle ids

    numParticles = numX * numY * numZ;
    int index = 0;
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            for (int k = 0; k < numZ; k++) {
                particlePos[index++] = vec3(
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
                s[(i * fNumY + j) * fNumZ + k] = cellDensity;
            }
        }
    }
}

void FlipFluid::integrateParticles(const float &_dt, const vec3 &_gravity)
{
    for (int i = 0; i < numParticles; i++) {
        particleVel[i] += _dt * _gravity;
        particlePos[i] += particleVel[i] * _dt;
    }
}

void FlipFluid::pushParticlesApart(const float& _numIters)
{
    float colorDiffusionCoeff = 0.001;

    // particleCount stars per cell

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

    // fill stars into cells

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

    // push stars apart

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
                        first = firstCellParticle[cellNr];
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
                            float _s = 0.5 * (minDist - d) / d;
                            dx *= _s;
                            dy *= _s;
                            dz *= _s;
                            particlePos[i].x -= dx;
                            particlePos[i].y -= dy;
                            particlePos[i].z -= dz;
                            particlePos[id].x += dx;
                            particlePos[id].y += dy;
                            particlePos[id].z += dz;

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
            prevUVW[i] = uvw[i];
        }

        for (int i = 0; i < fNumCells; i++) {
            duvw[i] = vec3(0.0f);
            uvw[i] = vec3(0.0f);
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
                uvw[nr0][component] += pv * d0; duvw[nr0][component] += d0;
                uvw[nr1][component] += pv * d1; duvw[nr1][component] += d1;
                uvw[nr2][component] += pv * d2; duvw[nr2][component] += d2;
                uvw[nr3][component] += pv * d3; duvw[nr3][component] += d3;
                uvw[nr4][component] += pv * d4; duvw[nr4][component] += d4;
                uvw[nr5][component] += pv * d5; duvw[nr5][component] += d5;
                uvw[nr6][component] += pv * d6; duvw[nr6][component] += d6;
                uvw[nr7][component] += pv * d7; duvw[nr7][component] += d7;
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

                float _v = particleVel[i].v[component];
                float _d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                           valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (_d > 0.0) {
                    float picV = (valid0 * d0 * uvw[nr0][component] + valid1 * d1 * uvw[nr1][component] +
                                  valid2 * d2 * uvw[nr2][component] + valid3 * d3 * uvw[nr3][component] +
                                  valid4 * d4 * uvw[nr4][component] + valid5 * d5 * uvw[nr5][component] +
                                  valid6 * d6 * uvw[nr6][component] + valid7 * d7 * uvw[nr7][component]) / _d;

                    float corr = (valid0 * d0 * (uvw[nr0][component] - prevUVW[nr0][component]) +
                                  valid1 * d1 * (uvw[nr1][component] - prevUVW[nr1][component]) +
                                  valid2 * d2 * (uvw[nr2][component] - prevUVW[nr2][component]) +
                                  valid3 * d3 * (uvw[nr3][component] - prevUVW[nr3][component]) +
                                  valid4 * d4 * (uvw[nr4][component] - prevUVW[nr4][component]) +
                                  valid5 * d5 * (uvw[nr5][component] - prevUVW[nr5][component]) +
                                  valid6 * d6 * (uvw[nr6][component] - prevUVW[nr6][component]) +
                                  valid7 * d7 * (uvw[nr7][component] - prevUVW[nr7][component])) / _d;

                    float flipV = _v + corr;

                    particleVel[i].v[component] = (1.0 - _flipRatio) * picV + _flipRatio * flipV;
                }
            }
        }

        if (_toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (duvw[i][component] > 0.0)
                    uvw[i][component] /= duvw[i][component];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int cellIndex = i * (n * fNumZ) + j * fNumZ + k;
                        int solid = cellType[cellIndex] == SOLID_CELL;

                        if (component == 0) {
                            if (solid || (i > 0 && cellType[(i - 1) * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL))
                                uvw[cellIndex].x = 0.0;
                        }
                        else if (component == 1) {
                            if (solid || (j > 0 && cellType[i * (n * fNumZ) + (j - 1) * fNumZ + k] == SOLID_CELL))
                                uvw[cellIndex].y = 0.0;
                        }
                        else { // component == 2
                            if (solid || (k > 0 && cellType[i * (n * fNumZ) + j * fNumZ + (k - 1)] == SOLID_CELL))
                                uvw[cellIndex].z = 0.0;
                        }
                    }
                }
            }
        }
    }
}

void FlipFluid::solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        p[i] = 0.0;
        prevUVW[i] = uvw[i];
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
                    float _s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (_s == 0.0)
                        continue;

                    float div = uvw[right].x - uvw[center].x + uvw[top].y - uvw[center].y + uvw[front].z - uvw[center].z;

                    if (particleRestDensity > 0.0 && _compensateDrift) {
                        float _k = 1.0;
                        float compression = particleDensity[i * (n * fNumZ) + j * fNumZ + k] - particleRestDensity;
                        if (compression > 0.0)
                            div = div - _k * compression;
                    }

                    float _p = -div / _s * _overRelaxation;
                    p[center] += cp * _p;

                    uvw[center].x -= sx0 * _p;
                    uvw[right].x += sx1 * _p;
                    uvw[center].y -= sy0 * _p;
                    uvw[top].y += sy1 * _p;
                    uvw[center].z -= sz0 * _p;
                    uvw[front].z += sz1 * _p;
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

    //updateParticleColors();
}