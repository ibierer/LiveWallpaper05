//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
#define LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H


class SimpleNBodySimulation : public Simulation {
public:

    static const int NUM_CACHE_CHUNKS = 32;

    static const int STARS_PER_CHUNK = 16;

    static const int COUNT = 512;

    static const int OFFSET_ATTRIBUTE_LOCATION = 2;

    struct cacheChunk { // 384 bytes
        Particle particles[STARS_PER_CHUNK];
    };

    struct __attribute__((aligned(128))) SimpleNBodySimulationData { // 12,288 bytes
        union {
            Particle particles[COUNT]; // CPU computation data
            cacheChunk chunks[NUM_CACHE_CHUNKS]; // GPU computation data
        };
    };

    // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
    string computeShaderCode[1000] = {
            "#version 320 es\n",
            "const uint STARS_PER_CHUNK = " + std::to_string(STARS_PER_CHUNK) + "u;",
            "const uint NUM_CACHE_CHUNKS = uint(" + std::to_string(NUM_CACHE_CHUNKS) + ");\n",
            "const uint COUNT = " + std::to_string(COUNT) + "u;\n",
            "struct Particle {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "};\n",
            "struct cacheChunk{\n",
            "    Particle particles[" + std::to_string(STARS_PER_CHUNK) + "];\n",
            "};\n",
            "layout(packed, binding = " + std::to_string(OFFSET_ATTRIBUTE_LOCATION) + ") buffer destBuffer{\n",
            "	  cacheChunk chunks[" + std::to_string(NUM_CACHE_CHUNKS) + "];\n",
            "} outBuffer;\n",
            "uniform float t;\n",
            "uint task;\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "void main(){\n",
            "	  task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
            "	  if(task < NUM_CACHE_CHUNKS && t > 0.0f){\n",
            "	  	vec3 gravitySum[STARS_PER_CHUNK];\n",
            "	  	for(uint type = 0u; type < STARS_PER_CHUNK && STARS_PER_CHUNK * task + type < COUNT; type++){\n",
            "	  		gravitySum[type] = vec3(0.0f, 0.0f, 0.0f);\n",
            "	  		for(uint j = 0u; j < NUM_CACHE_CHUNKS; j++){\n",
            "	  			for(uint k = 0u; k < STARS_PER_CHUNK && STARS_PER_CHUNK * j + k < COUNT; k++){\n",
            "	  				if(STARS_PER_CHUNK * j + k == STARS_PER_CHUNK * task + type)\n",
            "	  					continue;\n",
            "	  				vec3 difference = outBuffer.chunks[j].particles[k].position - outBuffer.chunks[task].particles[type].position;\n",
            "	  				float differenceSquared = dot(difference, difference);\n",
            "	  				float distance = sqrt(differenceSquared);\n",
            "	  				gravitySum[type] += difference / distance / differenceSquared;\n",
            "	  			}\n",
            "	  		}\n",
            "	  	}\n",
            "	  	barrier();\n",
            "	  	for(uint type = 0u; type < STARS_PER_CHUNK && STARS_PER_CHUNK * task + type < COUNT; type++){\n",
            "	  		outBuffer.chunks[task].particles[type].velocity += gravitySum[type];\n",
            "	  		outBuffer.chunks[task].particles[type].position += outBuffer.chunks[task].particles[type].velocity;\n",
            "	  	}\n",
            "	  }\n",
            "}"
    };

    SimpleNBodySimulationData* data;

    double t;

    SimpleNBodySimulation(){

    }

    ~SimpleNBodySimulation(){

    }

    void initialize(const ComputationOptions& computationOption);

    void simulate(bool pushDataToGPU, bool retrieveDataFromGPU);

    ComputationOptions getComputationOption();

    void pushData2GPU();

private:

    float dt;

    void simulateOnCPU();

    void simulateOnGPU(bool pushDataToGPU, bool retrieveDataFromGPU);

    float getRandomFloat(float x);

    bool seed();

};


#endif //LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
