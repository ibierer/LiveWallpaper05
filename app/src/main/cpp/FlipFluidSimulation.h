//
// Created by Immanuel Bierer on 5/4/2025.
//

#ifndef LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H
#define LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H


using std::to_string;

class FlipFluidSimulation : public Simulation {
public:

    double t;

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

    static const int COUNT = 4600;

    static const int PARTICLES_PER_CHUNK = COUNT / 1024 + (COUNT % 1024 > 0); // Round up after division

    static const int NUM_CACHE_CHUNKS = 1024;

    static const int DEFAULT_INDEX_BUFFER_BINDING = 0;

    static const int OFFSET_ATTRIBUTE_LOCATION = 2;

    static const int VELOCITY_ATTRIBUTE_LOCATION = 3;

    struct ParticleInfo : Particle { // 32 bytes

        int cellParticleId;

        int padding;

    };

    struct fCell { // 64 bytes

        vec3 uvw;       // Combined velocity field

        vec3 duvw;      // Combined velocity change

        vec3 prevUVW;   // Combined previous velocity

        float p;

        float s;

        int cellType;

        float particleDensity;

        float padding[3];

    };

    struct pCell { // 8 bytes

        int numCellParticles; // Max = 6

        int firstCellParticle;

    };

    struct __attribute__((aligned(128))) cacheLine { // 128 bytes
        union {
            ParticleInfo particles[4]; // per-particle data
            fCell fCells[2]; // velocity field cells
            pCell pCells[16]; // per-cell information
        };
    };

    struct __attribute__((aligned(128))) FlipFluidSimulationData {
        union {
            ParticleInfo particles[maxParticles]; // per-particle data
            fCell fCells[fNumCells]; // velocity field cells
            pCell pCells[pNumCells + 1]; // per-cell information
        };
    };

    FlipFluidSimulationData* data;

    // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
    string computeShaderCode[1000] = {
            View::ES_VERSION,
            "const uint PARTICLES_PER_CHUNK = " + to_string(PARTICLES_PER_CHUNK) + "u;",
            "const uint NUM_CACHE_CHUNKS = uint(" + to_string(NUM_CACHE_CHUNKS) + ");\n",
            "const uint COUNT = " + to_string(COUNT) + "u;\n",
            "struct ParticleInfo {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "    int cellParticleId;\n",
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
            "    float padding[3];\n",
            "};\n",
            "struct pCell {\n",
            "    int numCellParticles; // Max = 6\n",
            "    int firstCellParticle;\n",
            "};\n",
            "struct cacheChunk {\n",
            "    ParticleInfo particles[" + to_string(PARTICLES_PER_CHUNK) + "];\n",
            "};\n",
            "layout(packed, binding = " + to_string(DEFAULT_INDEX_BUFFER_BINDING) + ") buffer destBuffer {\n",
            "	  ParticleInfo particles[" + to_string(COUNT) + "];\n",
            "	  fCell fCells[" + to_string(fNumCells) + "];\n",
            "	  pCell pCells[" + to_string(pNumCells + 1) + "];\n",
            "} outBuffer;\n",
            "uniform float t;\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "void main(){\n",
            "    uint task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
            "    for(uint i = 0u; i < PARTICLES_PER_CHUNK; i++){\n",
            "        uint offset = 1024u * i;\n",
            "        uint index = offset + task;\n",
            "        if(index > COUNT) {\n",
            "            break;\n",
            "        }\n",
            "        float theta = 0.001f * float(index) * t;\n",
            "        float theta2 = theta + 3.14159265 / 4.0;\n",
            "        outBuffer.particles[index].position += vec3(sin(theta), cos(theta), sin(t * 0.01f * float(index)));\n",
            "        outBuffer.particles[index].velocity = 10.0f * vec3(sin(theta2), cos(theta2), 0.0f);\n",
            "    }\n",
            "}\n",
    };

    FlipFluidSimulation(){

    }

    ~FlipFluidSimulation(){

    }

    void initialize(const ComputationOptions& computationOption);

    void simulate(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU);

    ComputationOptions getComputationOption();

    void pushData2GPU();

private:

    void simulateOnCPU();

    void simulateOnGPU(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU);

    float getRandomFloat(float x);

    bool seed();

};


#endif //LIVEWALLPAPER05_FLIPFLUIDSIMULATION_H
