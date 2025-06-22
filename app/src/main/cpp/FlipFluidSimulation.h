//
// Created by Immanuel Bierer on 5/4/2025.
//

#ifndef LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H
#define LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H


using std::to_string;

class FlipFluidSimulation : public Simulation {
public:

    double t;

    constexpr static float density = 1000.0f;
    constexpr static const int width = 1000;
    constexpr static const int height = 1000;
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
    constexpr static float particleRestDensity = 0.0;                                            // Set the rest density of the particles

    constexpr static float pInvSpacing = 1.0 / (2.2 * particleRadius);
    constexpr static int pNumX = simWidth * pInvSpacing + 1;
    constexpr static int pNumY = simHeight * pInvSpacing + 1;
    constexpr static int pNumZ = simDepth * pInvSpacing + 1;
    constexpr static int pNumCells = pNumX * pNumY * pNumZ;

    constexpr static const int maxParticles = numX * numY * numZ;

    constexpr static int numParticles = numX * numY * numZ;

    constexpr static float dt = 1.0 / 60.0;

    constexpr static float flipRatio = 0.9f;

    constexpr static int numPressureIters = 50;

    constexpr static int numParticleIters = 2;

    constexpr static int frameNr = 0;

    constexpr static float overRelaxation = 1.9f;

    constexpr static bool compensateDrift = true;

    constexpr static bool separateParticles = true;

    constexpr static const int FLUID_CELL = 0;

    constexpr static const int AIR_CELL = 1;

    constexpr static const int SOLID_CELL = 2;

    static const int COUNT = numParticles;

    static const int PARTICLES_PER_CHUNK = COUNT / 1024 + (COUNT % 1024 > 0); // Round up after division

    static const int NUM_CACHE_CHUNKS = 1024;

    const uint NUM_GROUPS_X = 1u;
    const uint NUM_GROUPS_Y = 1u;
    const uint NUM_GROUPS_Z = 1u;

    const uint LOCAL_SIZE_X = 8u;
    const uint LOCAL_SIZE_Y = 8u;
    const uint LOCAL_SIZE_Z = 8u;

    static const int DEFAULT_INDEX_BUFFER_BINDING = 0;

    static const int OFFSET_ATTRIBUTE_LOCATION = 2;

    static const int VELOCITY_ATTRIBUTE_LOCATION = 3;

    struct ParticleInfo : Particle { // 32 bytes

        uint cellParticleId;

        int padding;

    };

    struct fCell { // 1536 bytes

        vec3 uvw;       // Combined velocity field

        vec3 duvw;      // Combined velocity change

        vec3 prevUVW;   // Combined previous velocity

        float p;

        float s;

        int cellType;

        float particleDensity;

        uint densityContributionsCounter;

        float densityContributions[50];

        uint velocityContributionsCounter;

        float velocityContributions[63];

        float accelerationContributions[63];

        uint padding;

    };

    struct pCell { // 128 bytes

        vec3 positions[7];

        uint indices[7];

        uint numCellParticles; // Max = 6

        uint firstCellParticle;

        uint padding[2];

    };

    struct __attribute__((aligned(128))) FlipFluidSimulationData {
        ParticleInfo particles[maxParticles]; // per-particle data
        fCell fCells[fNumCells]; // velocity field cells
        pCell pCells[pNumCells + 1]; // per-cell information
        uint logValues[4096][7];
    };

    FlipFluidSimulationData* data;

    // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
    string computeShaderCode[1000] = {
            View::ES_VERSION,
            "const uint PARTICLES_PER_CHUNK = " + to_string(PARTICLES_PER_CHUNK) + "u;",
            "const uint NUM_CACHE_CHUNKS = " + to_string(NUM_CACHE_CHUNKS) + "u;\n",
            "const uint numParticles = " + to_string(COUNT) + "u;\n",
            "const uint fNumCells = " + to_string(fNumCells) + "u;\n",
            "const uint pNumCells = " + to_string(pNumCells) + "u;\n",
            "const float pInvSpacing = " + to_string(pInvSpacing) + ";\n",
            "const float fInvSpacing = " + to_string(fInvSpacing) + ";\n",
            "const float density = " + to_string(density) + ";\n",
            "const float particleRadius = " + to_string(particleRadius) + ";\n",
            "float particleRestDensity = " + to_string(particleRestDensity) + ";\n",
            "const float h = " + to_string(h) + ";\n",
            "const float dt = " + to_string(dt) + ";\n",
            "const float flipRatio = " + to_string(flipRatio) + ";\n",
            "const float overRelaxation = " + to_string(overRelaxation) + ";\n",
            "const uint pNumX = " + to_string(pNumX) + "u;\n",
            "const uint pNumY = " + to_string(pNumY) + "u;\n",
            "const uint pNumZ = " + to_string(pNumZ) + "u;\n",
            "const uint fNumX = " + to_string(fNumX) + "u;\n",
            "const uint fNumY = " + to_string(fNumY) + "u;\n",
            "const uint fNumZ = " + to_string(fNumZ) + "u;\n",
            "const int FLUID_CELL = " + to_string(FLUID_CELL) + ";\n",
            "const int AIR_CELL = " + to_string(AIR_CELL) + ";\n",
            "const int SOLID_CELL = " + to_string(SOLID_CELL) + ";\n",
            "const int numParticleIters = " + to_string(numParticleIters) + ";\n",
            "const int numPressureIters = " + to_string(numPressureIters) + ";\n",
            "bool separateParticles = bool(" + to_string(separateParticles) + ");\n",
            "bool compensateDrift = bool(" + to_string(compensateDrift) + ");\n",
            "uniform float t;\n",
            "uniform vec3 acceleration;\n",
            "struct ParticleInfo {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "    uint cellParticleId;\n",
            "    int padding;\n",
            "};\n",
            "struct fCell {\n",
            "    vec3 uvw;       // Combined velocity field\n",
            "    vec3 duvw;      // Combined velocity change\n",
            "    vec3 prevUVW;   // Combined previous velocity\n",
            "    float p;\n",
            "    float s;\n",
            "    int cellType;\n",
            "    float particleDensity;\n",
            "    uint densityContributionsCounter;\n",
            "    float densityContributions[50];\n",
            "    uint velocityContributionsCounter;\n",
            "    float velocityContributions[63];\n",
            "    float accelerationContributions[63];\n",
            "    uint padding;\n",
            "};\n",
            "struct pCell {\n",
            "    vec3 positions[7];\n",
            "    uint indices[7];\n",
            "    uint numCellParticles; // Max = 6\n",
            "    uint firstCellParticle;\n",
            "    uint padding[2];\n",
            "};\n",
            "layout(packed, binding = " + to_string(DEFAULT_INDEX_BUFFER_BINDING) + ") buffer destBuffer {\n",
            "	  ParticleInfo particles[numParticles];\n",
            "	  fCell fCells[fNumCells];\n",
            "	  pCell pCells[pNumCells + 1u];\n",
            "	  uint logValues[4096][7];\n",
            "} outBuffer;\n",
            "const uvec3 num_groups = uvec3(" + to_string(NUM_GROUPS_X) + ", " + to_string(NUM_GROUPS_Y) + ", " + to_string(NUM_GROUPS_Z) + ");\n",
            "layout(local_size_x = " + to_string(LOCAL_SIZE_X) + ", local_size_y = " + to_string(LOCAL_SIZE_Y) + ", local_size_z = " + to_string(LOCAL_SIZE_Z) + ") in;\n",
            "const uint invocationsPerWorkGroup = gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z;\n",
            "const uint numWorkGroups = num_groups.x * num_groups.y * num_groups.z;\n",
            "const uint numGlobalInvocations = numWorkGroups * invocationsPerWorkGroup;\n",
            "const uint ceilOfParticlesPerInvocation = (numParticles + numGlobalInvocations - 1u) / numGlobalInvocations;\n",
            "const uint ceilOfPCellsPerInvocation = (pNumCells + numGlobalInvocations - 1u) / numGlobalInvocations;\n",
            "const uint ceilOfFCellsPerInvocation = (fNumCells + numGlobalInvocations - 1u) / numGlobalInvocations;\n",
            "uint getLocalInvocationIndex() {\n",
            "    return (gl_LocalInvocationID.x * gl_WorkGroupSize.y + gl_LocalInvocationID.y) * gl_WorkGroupSize.z + gl_LocalInvocationID.z;\n",
            "}\n",
            "uint getWorkGroupIndex() {\n",
            "    return (gl_WorkGroupID.x * num_groups.y + gl_WorkGroupID.y) * num_groups.z + gl_WorkGroupID.z;\n",
            "}\n",
            "uint getTask(){\n",
            "    return getWorkGroupIndex() * invocationsPerWorkGroup + getLocalInvocationIndex();\n",
            "}\n",
            "uint task = getTask();\n",
            "// Iterate over particles\n",
            "void integrateParticles(const float dt, const vec3 gravity){\n",
            //"    for (uint i = task; i < numParticles; i += numGlobalInvocations) {\n",
            "    for (uint i = ceilOfParticlesPerInvocation * task; i < ceilOfParticlesPerInvocation * (task + 1u) && i < numParticles; i++) {\n",
            "        outBuffer.particles[i].velocity += dt * gravity;\n",
            "        outBuffer.particles[i].position += outBuffer.particles[i].velocity * dt;\n",
            "    }\n",
            "}\n",
            "// Thrashing issue: reading and writing from and to pCells and particles. Non-parallelizable issue\n",
            "void pushParticlesApart(const int numIters){\n",
            "    float colorDiffusionCoeff = 0.001;\n",
            "    \n",
            "    // particleCount particles per cell\n",
            "    \n",
            //"    for (uint i = task; i < pNumCells; i += numGlobalInvocations) {\n",
            "    for (uint i = ceilOfPCellsPerInvocation * task; i < ceilOfPCellsPerInvocation * (task + 1u) && i < pNumCells; i++) {\n",
            "        outBuffer.pCells[i].numCellParticles = 0u;\n",
            "    }\n",
            "    barrier();\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (uint i = 0u; i < numParticles; i++) {\n",
            "            uint xi = uint(clamp(floor(outBuffer.particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1u)));\n",
            "            uint yi = uint(clamp(floor(outBuffer.particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1u)));\n",
            "            uint zi = uint(clamp(floor(outBuffer.particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1u)));\n",
            "            uint cellNr = (xi * pNumY + yi) * pNumZ + zi;\n",
            "            outBuffer.pCells[cellNr].numCellParticles++;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    // partial sums\n",
            "    \n",
            "    uint first = 0u;\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (uint i = 0u; i < pNumCells; i++) {\n",
            "            first += outBuffer.pCells[i].numCellParticles;\n",
            "            outBuffer.pCells[i].firstCellParticle = first;\n",
            "        }\n",
            "    }\n",
            "    if(task == 0u) {\n",
            "        outBuffer.pCells[pNumCells].firstCellParticle = first; // guard\n",
            "    }\n",
            "    \n",
            "    // fill particles into cells\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (uint i = 0u; i < numParticles; i++) {\n",
            "            uint xi = uint(clamp(floor(outBuffer.particles[i].position.x * pInvSpacing), 0.0f, float(pNumX - 1u)));\n",
            "            uint yi = uint(clamp(floor(outBuffer.particles[i].position.y * pInvSpacing), 0.0f, float(pNumY - 1u)));\n",
            "            uint zi = uint(clamp(floor(outBuffer.particles[i].position.z * pInvSpacing), 0.0f, float(pNumZ - 1u)));\n",
            "            uint cellNr = (xi * pNumY + yi) * pNumZ + zi;\n",
            "            outBuffer.pCells[cellNr].firstCellParticle--;\n",
            "            outBuffer.particles[outBuffer.pCells[cellNr].firstCellParticle].cellParticleId = i;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    // push particles apart\n",
            "    \n",
            "    float minDist = 2.0 * particleRadius;\n",
            "    float minDist2 = minDist * minDist;\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (int iter = 0; iter < numIters; iter++) {\n",
            "            \n",
            "            for (uint i = 0u; i < numParticles; i++) {\n",
            "                vec3 pxyz = outBuffer.particles[i].position;\n",
            "                uint pxi = uint(floor(outBuffer.particles[i].position.x * pInvSpacing));\n",
            "                uint pyi = uint(floor(outBuffer.particles[i].position.y * pInvSpacing));\n",
            "                uint pzi = uint(floor(outBuffer.particles[i].position.z * pInvSpacing));\n",
            "                uint x0 = max(pxi - 1u, 0u);\n",
            "                uint y0 = max(pyi - 1u, 0u);\n",
            "                uint z0 = max(pzi - 1u, 0u);\n",
            "                uint x1 = min(pxi + 1u, pNumX - 1u);\n",
            "                uint y1 = min(pyi + 1u, pNumY - 1u);\n",
            "                uint z1 = min(pzi + 1u, pNumZ - 1u);\n",
            "                \n",
            "                for (uint xi = x0; xi <= x1; xi++) {\n",
            "                    for (uint yi = y0; yi <= y1; yi++) {\n",
            "                        for (uint zi = z0; zi <= z1; zi++) {\n",
            "                            uint cellNr = (xi * pNumY + yi) * pNumZ + zi;\n",
            "                            first = outBuffer.pCells[cellNr].firstCellParticle;\n",
            "                            uint last = outBuffer.pCells[cellNr + 1u].firstCellParticle;\n",
            "                            for (uint j = first; j < last; j++) {\n",
            "                                uint id = outBuffer.particles[j].cellParticleId;\n",
            "                                if (id == i)\n",
            "                                    continue;\n",
            "                                vec3 dxyz = outBuffer.particles[id].position - pxyz;\n",
            "                                float d2 = dot(dxyz, dxyz);\n",
            "                                if (d2 > minDist2 || d2 == 0.0)\n",
            "                                    continue;\n",
            "                                float d = sqrt(d2);\n",
            "                                float _s = 0.5 * (minDist - d) / d;\n",
            "                                dxyz *= _s;\n",
            "                                outBuffer.particles[i].position -= dxyz;\n",
            "                                outBuffer.particles[id].position += dxyz;\n",
            "                            }\n",
            "                        }\n",
            "                    }\n",
            "                }\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "}\n",
            "// Iterate over particles\n",
            "void handleParticleCollisions(){\n",
            "    float r = particleRadius;\n",
            "    \n",
            "    float minX = h + r;\n",
            "    float maxX = float(fNumX - 1u) * h - r;\n",
            "    float minY = h + r;\n",
            "    float maxY = float(fNumY - 1u) * h - r;\n",
            "    float minZ = h + r;\n",
            "    float maxZ = float(fNumZ - 1u) * h - r;\n",
            "    \n",
            //"    for (uint i = task; i < numParticles; i += numGlobalInvocations) {\n",
            "    for (uint i = ceilOfParticlesPerInvocation * task; i < ceilOfParticlesPerInvocation * (task + 1u) && i < numParticles; i++) {\n",
            "        \n",
            "        // Clamp position and zero velocity on collision\n",
            "        if (outBuffer.particles[i].position.x < minX) {\n",
            "            outBuffer.particles[i].position.x = minX;\n",
            "            outBuffer.particles[i].velocity.x = 0.0f;\n",
            "        } else if (outBuffer.particles[i].position.x > maxX) {\n",
            "            outBuffer.particles[i].position.x = maxX;\n",
            "            outBuffer.particles[i].velocity.x = 0.0f;\n",
            "        }\n",
            "        \n",
            "        if (outBuffer.particles[i].position.y < minY) {\n",
            "            outBuffer.particles[i].position.y = minY;\n",
            "            outBuffer.particles[i].velocity.y = 0.0f;\n",
            "        } else if (outBuffer.particles[i].position.y > maxY) {\n",
            "            outBuffer.particles[i].position.y = maxY;\n",
            "            outBuffer.particles[i].velocity.y = 0.0f;\n",
            "        }\n",
            "        \n",
            "        if (outBuffer.particles[i].position.z < minZ) {\n",
            "            outBuffer.particles[i].position.z = minZ;\n",
            "            outBuffer.particles[i].velocity.z = 0.0f;\n",
            "        } else if (outBuffer.particles[i].position.z > maxZ) {\n",
            "            outBuffer.particles[i].position.z = maxZ;\n",
            "            outBuffer.particles[i].velocity.z = 0.0f;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "}\n",
            "// Iterate over fCells, must compute 2 sequential sums\n",
            "void updateParticleDensity(){\n",
            "    uint n = fNumY;\n",
            "    float h1 = fInvSpacing;\n",
            "    float h2 = 0.5 * h;\n",
            "    \n",
            //"    for (uint i = task; i < fNumCells; i += numGlobalInvocations) {\n",
            "    for (uint i = ceilOfFCellsPerInvocation * task; i < ceilOfFCellsPerInvocation * (task + 1u) && i < fNumCells; i++) {\n",
            "        outBuffer.fCells[i].particleDensity = 0.0;\n",
            "    }\n",
            "    barrier();\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (uint i = 0u; i < numParticles; i++) {\n",
            "            float x = clamp(outBuffer.particles[i].position.x, h, float(fNumX - 1u) * h);\n",
            "            float y = clamp(outBuffer.particles[i].position.y, h, float(fNumY - 1u) * h);\n",
            "            float z = clamp(outBuffer.particles[i].position.z, h, float(fNumZ - 1u) * h);\n",
            "            \n",
            "            uint x0 = uint(floor((x - h2) * h1));\n",
            "            float tx = ((x - h2) - float(x0) * h) * h1;\n",
            "            uint x1 = min(x0 + 1u, fNumX - 2u);\n",
            "            \n",
            "            uint y0 = uint(floor((y - h2) * h1));\n",
            "            float ty = ((y - h2) - float(y0) * h) * h1;\n",
            "            uint y1 = min(y0 + 1u, fNumY - 2u);\n",
            "            \n",
            "            uint z0 = uint(floor((z - h2) * h1));\n",
            "            float tz = ((z - h2) - float(z0) * h) * h1;\n",
            "            uint z1 = min(z0 + 1u, fNumZ - 2u);\n",
            "            \n",
            "            float sx = 1.0 - tx;\n",
            "            float sy = 1.0 - ty;\n",
            "            float sz = 1.0 - tz;\n",
            "            \n",
            "            if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) outBuffer.fCells[(x0 * n + y0) * fNumZ + z0].particleDensity += sx * sy * sz;\n",
            "            if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) outBuffer.fCells[(x1 * n + y0) * fNumZ + z0].particleDensity += tx * sy * sz;\n",
            "            if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) outBuffer.fCells[(x1 * n + y1) * fNumZ + z0].particleDensity += tx * ty * sz;\n",
            "            if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) outBuffer.fCells[(x0 * n + y1) * fNumZ + z0].particleDensity += sx * ty * sz;\n",
            "            if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) outBuffer.fCells[(x0 * n + y0) * fNumZ + z1].particleDensity += sx * sy * tz;\n",
            "            if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) outBuffer.fCells[(x1 * n + y0) * fNumZ + z1].particleDensity += tx * sy * tz;\n",
            "            if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) outBuffer.fCells[(x1 * n + y1) * fNumZ + z1].particleDensity += tx * ty * tz;\n",
            "            if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) outBuffer.fCells[(x0 * n + y1) * fNumZ + z1].particleDensity += sx * ty * tz;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        if (particleRestDensity == 0.0) {\n",
            "            float sum = 0.0;\n",
            "            int numFluidCells = 0;\n",
            "            \n",
            "            for (uint i = 0u; i < fNumCells; i++) {\n",
            "                if (outBuffer.fCells[i].cellType == FLUID_CELL) {\n",
            "                    sum += outBuffer.fCells[i].particleDensity;\n",
            "                    numFluidCells++;\n",
            "                }\n",
            "            }\n",
            "            \n",
            "            if (numFluidCells > 0)\n",
            "                particleRestDensity = sum / float(numFluidCells);\n",
            "        }\n",
            "    }\n",
            "}\n",
            "// Thrashing issue: reading and writing from and to fCells and particles\n",
            "void transferVelocities(const bool toGrid, const float flipRatio){\n",
            "    uint n = fNumY;\n",
            "    float h1 = fInvSpacing;\n",
            "    float h2 = 0.5f * h;\n",
            "    \n",
            "    if (toGrid) {\n",
            //"        for (uint i = task; i < fNumCells; i += numGlobalInvocations) {\n",
            "        for (uint i = ceilOfFCellsPerInvocation * task; i < ceilOfFCellsPerInvocation * (task + 1u) && i < fNumCells; i++) {\n",
            "            outBuffer.fCells[i].prevUVW = outBuffer.fCells[i].uvw;\n",
            "            outBuffer.fCells[i].duvw = vec3(0.0f);\n",
            "            outBuffer.fCells[i].uvw = vec3(0.0f);\n",
            "            outBuffer.fCells[i].cellType = outBuffer.fCells[i].s == 0.0 ? SOLID_CELL : AIR_CELL;\n",
            "        }\n",
            "        barrier();\n",
            "        \n",
            //"        for (uint i = task; i < numParticles; i += numGlobalInvocations) {\n",
            "        for (uint i = ceilOfParticlesPerInvocation * task; i < ceilOfParticlesPerInvocation * (task + 1u) && i < numParticles; i++) {\n",
            "            uint xi = uint(clamp(floor(outBuffer.particles[i].position.x * h1), 0.0f, float(fNumX - 1u)));\n",
            "            uint yi = uint(clamp(floor(outBuffer.particles[i].position.y * h1), 0.0f, float(fNumY - 1u)));\n",
            "            uint zi = uint(clamp(floor(outBuffer.particles[i].position.z * h1), 0.0f, float(fNumZ - 1u)));\n",
            "            \n",
            "            uint cellNr = (xi * n + yi) * fNumZ + zi;\n",
            "            if (outBuffer.fCells[cellNr].cellType == AIR_CELL)\n",
            "                outBuffer.fCells[cellNr].cellType = FLUID_CELL;\n",
            "        }\n",
            "        barrier();\n",
            "    }\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (int component = 0; component < 3; component++) {\n",
            "            float dx = (component == 0) ? 0.0 : h2;\n",
            "            float dy = (component == 1) ? 0.0 : h2;\n",
            "            float dz = (component == 2) ? 0.0 : h2;\n",
            "            \n",
            "            for (uint i = 0u; i < numParticles; i++) {\n",
            "                float x = clamp(outBuffer.particles[i].position.x, h, float(fNumX - 1u) * h);\n",
            "                float y = clamp(outBuffer.particles[i].position.y, h, float(fNumY - 1u) * h);\n",
            "                float z = clamp(outBuffer.particles[i].position.z, h, float(fNumZ - 1u) * h);\n",
            "                \n",
            "                uint x0 = uint(min(floor((x - dx) * h1), float(fNumX - 2u)));\n",
            "                float tx = (x - dx - float(x0) * h) * h1;\n",
            "                uint x1 = min(x0 + 1u, fNumX - 2u);\n",
            "                \n",
            "                uint y0 = uint(min(floor((y - dy) * h1), float(fNumY - 2u)));\n",
            "                float ty = (y - dy - float(y0) * h) * h1;\n",
            "                uint y1 = min(y0 + 1u, fNumY - 2u);\n",
            "                \n",
            "                uint z0 = uint(min(floor((z - dz) * h1), float(fNumZ - 2u)));\n",
            "                float tz = (z - dz - float(z0) * h) * h1;\n",
            "                uint z1 = min(z0 + 1u, fNumZ - 2u);\n",
            "                \n",
            "                float sx = 1.0 - tx;\n",
            "                float sy = 1.0 - ty;\n",
            "                float sz = 1.0 - tz;\n",
            "                \n",
            "                float d0 = sx * sy * sz;\n",
            "                float d1 = tx * sy * sz;\n",
            "                float d2 = tx * ty * sz;\n",
            "                float d3 = sx * ty * sz;\n",
            "                float d4 = sx * sy * tz;\n",
            "                float d5 = tx * sy * tz;\n",
            "                float d6 = tx * ty * tz;\n",
            "                float d7 = sx * ty * tz;\n",
            "                \n",
            "                uint nr0 = (x0 * n + y0) * fNumZ + z0;\n",
            "                uint nr1 = (x1 * n + y0) * fNumZ + z0;\n",
            "                uint nr2 = (x1 * n + y1) * fNumZ + z0;\n",
            "                uint nr3 = (x0 * n + y1) * fNumZ + z0;\n",
            "                uint nr4 = (x0 * n + y0) * fNumZ + z1;\n",
            "                uint nr5 = (x1 * n + y0) * fNumZ + z1;\n",
            "                uint nr6 = (x1 * n + y1) * fNumZ + z1;\n",
            "                uint nr7 = (x0 * n + y1) * fNumZ + z1;\n",
            "                \n",
            "                if (toGrid) {\n",
            "                    float pv = outBuffer.particles[i].velocity[component];\n",
            "                    outBuffer.fCells[nr0].uvw[component] += pv * d0; outBuffer.fCells[nr0].duvw[component] += d0;\n",
            "                    outBuffer.fCells[nr1].uvw[component] += pv * d1; outBuffer.fCells[nr1].duvw[component] += d1;\n",
            "                    outBuffer.fCells[nr2].uvw[component] += pv * d2; outBuffer.fCells[nr2].duvw[component] += d2;\n",
            "                    outBuffer.fCells[nr3].uvw[component] += pv * d3; outBuffer.fCells[nr3].duvw[component] += d3;\n",
            "                    outBuffer.fCells[nr4].uvw[component] += pv * d4; outBuffer.fCells[nr4].duvw[component] += d4;\n",
            "                    outBuffer.fCells[nr5].uvw[component] += pv * d5; outBuffer.fCells[nr5].duvw[component] += d5;\n",
            "                    outBuffer.fCells[nr6].uvw[component] += pv * d6; outBuffer.fCells[nr6].duvw[component] += d6;\n",
            "                    outBuffer.fCells[nr7].uvw[component] += pv * d7; outBuffer.fCells[nr7].duvw[component] += d7;\n",
            "                }\n",
            "                else {\n",
            "                    uint offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1u);\n",
            "                    \n",
            "                    float valid0 = (outBuffer.fCells[nr0].cellType != AIR_CELL || outBuffer.fCells[nr0 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid1 = (outBuffer.fCells[nr1].cellType != AIR_CELL || outBuffer.fCells[nr1 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid2 = (outBuffer.fCells[nr2].cellType != AIR_CELL || outBuffer.fCells[nr2 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid3 = (outBuffer.fCells[nr3].cellType != AIR_CELL || outBuffer.fCells[nr3 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid4 = (outBuffer.fCells[nr4].cellType != AIR_CELL || outBuffer.fCells[nr4 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid5 = (outBuffer.fCells[nr5].cellType != AIR_CELL || outBuffer.fCells[nr5 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid6 = (outBuffer.fCells[nr6].cellType != AIR_CELL || outBuffer.fCells[nr6 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    float valid7 = (outBuffer.fCells[nr7].cellType != AIR_CELL || outBuffer.fCells[nr7 - offset].cellType != AIR_CELL) ? 1.0 : 0.0;\n",
            "                    \n",
            "                    float _v = outBuffer.particles[i].velocity[component];\n",
            "                    float _d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +\n",
            "                               valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;\n",
            "                    \n",
            "                    if (_d > 0.0) {\n",
            "                        float picV = (valid0 * d0 * outBuffer.fCells[nr0].uvw[component] + valid1 * d1 * outBuffer.fCells[nr1].uvw[component] +\n",
            "                                      valid2 * d2 * outBuffer.fCells[nr2].uvw[component] + valid3 * d3 * outBuffer.fCells[nr3].uvw[component] +\n",
            "                                      valid4 * d4 * outBuffer.fCells[nr4].uvw[component] + valid5 * d5 * outBuffer.fCells[nr5].uvw[component] +\n",
            "                                      valid6 * d6 * outBuffer.fCells[nr6].uvw[component] + valid7 * d7 * outBuffer.fCells[nr7].uvw[component]) / _d;\n",
            "                        \n",
            "                        float corr = (valid0 * d0 * (outBuffer.fCells[nr0].uvw[component] - outBuffer.fCells[nr0].prevUVW[component]) +\n",
            "                                      valid1 * d1 * (outBuffer.fCells[nr1].uvw[component] - outBuffer.fCells[nr1].prevUVW[component]) +\n",
            "                                      valid2 * d2 * (outBuffer.fCells[nr2].uvw[component] - outBuffer.fCells[nr2].prevUVW[component]) +\n",
            "                                      valid3 * d3 * (outBuffer.fCells[nr3].uvw[component] - outBuffer.fCells[nr3].prevUVW[component]) +\n",
            "                                      valid4 * d4 * (outBuffer.fCells[nr4].uvw[component] - outBuffer.fCells[nr4].prevUVW[component]) +\n",
            "                                      valid5 * d5 * (outBuffer.fCells[nr5].uvw[component] - outBuffer.fCells[nr5].prevUVW[component]) +\n",
            "                                      valid6 * d6 * (outBuffer.fCells[nr6].uvw[component] - outBuffer.fCells[nr6].prevUVW[component]) +\n",
            "                                      valid7 * d7 * (outBuffer.fCells[nr7].uvw[component] - outBuffer.fCells[nr7].prevUVW[component])) / _d;\n",
            "                        \n",
            "                        float flipV = _v + corr;\n",
            "                        \n",
            "                        outBuffer.particles[i].velocity[component] = (1.0 - flipRatio) * picV + flipRatio * flipV;\n",
            "                    }\n",
            "                }\n",
            "            }\n",
            "            \n",
            "            if (toGrid) {\n",
            "                for (uint i = 0u; i < fNumCells; i++) {\n",
            "                    if (outBuffer.fCells[i].duvw[component] > 0.0)\n",
            "                        outBuffer.fCells[i].uvw[component] /= outBuffer.fCells[i].duvw[component];\n",
            "                }\n",
            "                \n",
            "                // Restore solid cells\n",
            "                for (uint i = 0u; i < fNumX; i++) {\n",
            "                    for (uint j = 0u; j < fNumY; j++) {\n",
            "                        for (uint k = 0u; k < fNumZ; k++) {\n",
            "                            uint cellIndex = (i * n + j) * fNumZ + k;\n",
            "                            bool solid = outBuffer.fCells[cellIndex].cellType == SOLID_CELL;\n",
            "                            \n",
            "                            if (component == 0) {\n",
            "                                if (solid || (i > 0u && outBuffer.fCells[((i - 1u) * n + j) * fNumZ + k].cellType == SOLID_CELL))\n",
            "                                    outBuffer.fCells[cellIndex].uvw.x = 0.0;\n",
            "                            }\n",
            "                            else if (component == 1) {\n",
            "                                if (solid || (j > 0u && outBuffer.fCells[(i * n + (j - 1u)) * fNumZ + k].cellType == SOLID_CELL))\n",
            "                                    outBuffer.fCells[cellIndex].uvw.y = 0.0;\n",
            "                            }\n",
            "                            else { // component == 2\n",
            "                                if (solid || (k > 0u && outBuffer.fCells[(i * n + j) * fNumZ + (k - 1u)].cellType == SOLID_CELL))\n",
            "                                    outBuffer.fCells[cellIndex].uvw.z = 0.0;\n",
            "                            }\n",
            "                        }\n",
            "                    }\n",
            "                }\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "}\n",
            "// Iterate over fCells\n",
            "void solveIncompressibility(const int numIters, const float dt, const float overRelaxation, const bool compensateDrift){\n",
            //"    for (uint i = task; i < fNumCells; i += numGlobalInvocations) {\n",
            "    for (uint i = ceilOfFCellsPerInvocation * task; i < ceilOfFCellsPerInvocation * (task + 1u) && i < fNumCells; i++) {\n",
            "        outBuffer.fCells[i].p = 0.0;\n",
            "        outBuffer.fCells[i].prevUVW = outBuffer.fCells[i].uvw;\n",
            "    }\n",
            "    barrier();\n",
            "    \n",
            "    uint n = fNumY;\n",
            "    float cp = density * h / dt;\n",
            "    \n",
            "    if(task == 0u) {\n",
            "        for (int iter = 0; iter < numIters; iter++) {\n",
            "            for (uint i = 1u; i < fNumX - 1u; i++) {\n",
            "                for (uint j = 1u; j < fNumY - 1u; j++) {\n",
            "                    for (uint k = 1u; k < fNumZ - 1u; k++) {\n",
            "                        \n",
            "                        if (outBuffer.fCells[(i * n + j) * fNumZ + k].cellType != FLUID_CELL)\n",
            "                            continue;\n",
            "                        \n",
            "                        uint center = (i * n + j) * fNumZ + k;\n",
            "                        uint left = ((i - 1u) * n + j) * fNumZ + k;\n",
            "                        uint right = ((i + 1u) * n + j) * fNumZ + k;\n",
            "                        uint bottom = (i * n + j - 1u) * fNumZ + k;\n",
            "                        uint top = (i * n + j + 1u) * fNumZ + k;\n",
            "                        uint back = (i * n + j) * fNumZ + k - 1u;\n",
            "                        uint front = (i * n + j) * fNumZ + k + 1u;\n",
            "                        \n",
            "                        float sx0 = outBuffer.fCells[left].s;\n",
            "                        float sx1 = outBuffer.fCells[right].s;\n",
            "                        float sy0 = outBuffer.fCells[bottom].s;\n",
            "                        float sy1 = outBuffer.fCells[top].s;\n",
            "                        float sz0 = outBuffer.fCells[back].s;\n",
            "                        float sz1 = outBuffer.fCells[front].s;\n",
            "                        float _s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;\n",
            "                        \n",
            "                        if (_s == 0.0)\n",
            "                            continue;\n",
            "                        \n",
            "                        float div = outBuffer.fCells[right].uvw.x - outBuffer.fCells[center].uvw.x + outBuffer.fCells[top].uvw.y - outBuffer.fCells[center].uvw.y + outBuffer.fCells[front].uvw.z - outBuffer.fCells[center].uvw.z;\n",
            "                        \n",
            "                        if (particleRestDensity > 0.0 && compensateDrift) {\n",
            "                            float _k = 1.0;\n",
            "                            float compression = outBuffer.fCells[(i * n + j) * fNumZ + k].particleDensity - particleRestDensity;\n",
            "                            if (compression > 0.0)\n",
            "                                div -= _k * compression;\n",
            "                        }\n",
            "                        \n",
            "                        float _p = -div / _s * overRelaxation;\n",
            "                        outBuffer.fCells[center].p += cp * _p;\n",
            "                        \n",
            "                        outBuffer.fCells[center].uvw.x -= sx0 * _p;\n",
            "                        outBuffer.fCells[right].uvw.x += sx1 * _p;\n",
            "                        outBuffer.fCells[center].uvw.y -= sy0 * _p;\n",
            "                        outBuffer.fCells[top].uvw.y += sy1 * _p;\n",
            "                        outBuffer.fCells[center].uvw.z -= sz0 * _p;\n",
            "                        outBuffer.fCells[front].uvw.z += sz1 * _p;\n",
            "                    }\n",
            "                }\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "}\n",
            "\n",
            "void simulate(const vec3 gravity){\n",
            "    int numSubSteps = 1;\n",
            "    float sdt = dt / float(numSubSteps);\n",
            "    \n",
            "    for (int step = 0; step < numSubSteps; step++) {\n",
            "        integrateParticles(sdt, gravity);\n",
            "        barrier();\n",
            "        if (separateParticles)\n",
            "            pushParticlesApart(numParticleIters);\n",
            "        barrier();\n",
            "        handleParticleCollisions();\n",
            "        barrier();\n",
            "        transferVelocities(true, flipRatio);\n",
            "        barrier();\n",
            "        updateParticleDensity();\n",
            "        barrier();\n",
            "        solveIncompressibility(numPressureIters, sdt, overRelaxation, compensateDrift);\n",
            "        barrier();\n",
            "        transferVelocities(false, flipRatio);\n",
            "    }\n",
            "}\n",
            "void main(){\n",
            //"    uint task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
            "    simulate(acceleration);\n",
            "}\n",
    };

    FlipFluidSimulation(){

    }

    ~FlipFluidSimulation(){

    }

    void initialize(const ComputationOptions& computationOption);

    void simulate(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU, const vec3& forceVector);

    ComputationOptions getComputationOption();

    void pushData2GPU();

private:

    void simulateOnCPU();

    void simulateOnGPU(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU, const vec3& forceVector);

    float getRandomFloat(float x);

    bool seed();

};


#endif //LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H
