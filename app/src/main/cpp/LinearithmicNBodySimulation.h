//
// Created by camer on 2/29/2024.
//

#ifndef LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATION_H
#define LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATION_H


using std::to_string;

class LinearithmicNBodySimulation : public Simulation {
public:

    static const int NUM_CACHE_CHUNKS = 60;

    static const int PARTICLES_PER_CHUNK = 16;

    static const int COUNT = NUM_CACHE_CHUNKS * PARTICLES_PER_CHUNK;

    static const int DEFAULT_INDEX_BUFFER_BINDING = 0;

    static const int OFFSET_ATTRIBUTE_LOCATION = 2;

    static const int VELOCITY_ATTRIBUTE_LOCATION = 3;

    struct __attribute__((aligned(128))) cacheChunk { // 512 bytes
        Particle particles[PARTICLES_PER_CHUNK];
        float padding[(512 - sizeof(particles)) / sizeof(float)];
    };

    struct __attribute__((aligned(128))) LinearithmicNBodySimulationData {
        union {
            Star stars[COUNT]; // CPU computation data
            cacheChunk chunks[NUM_CACHE_CHUNKS]; // GPU computation data
        };
    };

    // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
    string computeShaderCode[1000] = {
            View::ES_VERSION,
            "const uint PARTICLES_PER_CHUNK = " + to_string(PARTICLES_PER_CHUNK) + "u;",
            "const uint NUM_CACHE_CHUNKS = uint(" + to_string(NUM_CACHE_CHUNKS) + ");\n",
            "const uint COUNT = " + to_string(COUNT) + "u;\n",
            "struct Particle {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "};\n",
            "struct cacheChunk {\n",
            "    Particle stars[" + to_string(PARTICLES_PER_CHUNK) + "];\n",
            "    float padding[" + to_string((int) sizeof(cacheChunk::padding) / sizeof(float)) +
            "];\n",
            "};\n",
            "layout(packed, binding = " + to_string(DEFAULT_INDEX_BUFFER_BINDING) +
            ") buffer destBuffer {\n",
            "	  cacheChunk chunks[" + to_string(NUM_CACHE_CHUNKS) + "];\n",
            "} outBuffer;\n",
            "uniform float t;\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "void main(){\n",
            "    uint task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
            "    if(task >= NUM_CACHE_CHUNKS){\n",
            "        return;\n",
            "    }\n",
            "    vec3 gravitySum[PARTICLES_PER_CHUNK];\n",
            "    for(uint i = 0u; i < PARTICLES_PER_CHUNK; i++){\n",
            "        gravitySum[i] = vec3(0.0f, 0.0f, 0.0f);\n",
            "        for(uint j = 0u; j < NUM_CACHE_CHUNKS; j++){\n",
            "            for(uint k = 0u; k < PARTICLES_PER_CHUNK; k++){\n",
            "                if(PARTICLES_PER_CHUNK * j + k == PARTICLES_PER_CHUNK * task + i)\n",
            "                    continue;\n",
            "                vec3 difference = outBuffer.chunks[j].stars[k].position - outBuffer.chunks[task].stars[i].position;\n",
            "                float differenceSquared = dot(difference, difference);\n",
            "                float distance = sqrt(differenceSquared);\n",
            "                gravitySum[i] += difference / distance / differenceSquared;\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "    barrier();\n",
            "    for(uint i = 0u; i < PARTICLES_PER_CHUNK; i++){\n",
            "        outBuffer.chunks[task].stars[i].velocity += gravitySum[i];\n",
            "        outBuffer.chunks[task].stars[i].position += outBuffer.chunks[task].stars[i].velocity;\n",
            "    }\n",
            "}"
    };

    LinearithmicNBodySimulationData *data;

    vector<int> ids;

    double t;

    LinearithmicNBodySimulation() {}

    ~LinearithmicNBodySimulation() {}

    void initialize(const ComputationOptions &computationOption);

    void simulate(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU);

    ComputationOptions getComputationOption();

    void pushData2GPU();

private:

    float dt;

    void simulateOnGPU(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU);

    float getRandomFloat(float x);

    bool seed();

    Node *root = nullptr;

    vec4 conquerVolume(const vector<int> &ids, Node *node);

    void computeForcesOnCPULinearithmic();

    vec3 addForces(Node *node, int index);

    void integrate();

    int getCombo(bool xIsPos, bool yIsPos, bool zIsPos);

    std::tuple<bool, bool, bool> getInverseCombo(int combo);

    void finalIntegrate();

    vec4 conquerOnceMore(const vector<int> &ids, Node *node);

    vec3 addForcesFinal(Node *node, int index);

    void computeForcesOnCPULinearithmicFinal();
};


#endif //LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATION_H
