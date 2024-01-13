//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
#define LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H


class SimpleNBodySimulation : public Simulation {
public:

    static const int NUM_CACHE_CHUNKS = 32;

    static const int PARTICLES_PER_CHUNK = 16;

    static const int COUNT = NUM_CACHE_CHUNKS * PARTICLES_PER_CHUNK;

    static const int OFFSET_ATTRIBUTE_LOCATION = 2;

    static const int VELOCITY_ATTRIBUTE_LOCATION = 3;

    struct cacheChunk { // 384 bytes
        Particle particles[PARTICLES_PER_CHUNK];
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
            "const uint PARTICLES_PER_CHUNK = " + std::to_string(PARTICLES_PER_CHUNK) + "u;",
            "const uint NUM_CACHE_CHUNKS = uint(" + std::to_string(NUM_CACHE_CHUNKS) + ");\n",
            "const uint COUNT = " + std::to_string(COUNT) + "u;\n",
            "struct Particle {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "};\n",
            "struct cacheChunk{\n",
            "    Particle particles[" + std::to_string(PARTICLES_PER_CHUNK) + "];\n",
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
            "	  	  vec3 gravitySum[PARTICLES_PER_CHUNK];\n",
            "	  	  for(uint i = 0u; i < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * task + i < COUNT; i++){\n",
            "	  	      gravitySum[i] = vec3(0.0f, 0.0f, 0.0f);\n",
            "	  	      for(uint j = 0u; j < NUM_CACHE_CHUNKS; j++){\n",
            "	  	          for(uint k = 0u; k < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * j + k < COUNT; k++){\n",
            "	  	              if(PARTICLES_PER_CHUNK * j + k == PARTICLES_PER_CHUNK * task + i)\n",
            "	  	                  continue;\n",
            "	  	              vec3 difference = outBuffer.chunks[j].particles[k].position - outBuffer.chunks[task].particles[i].position;\n",
            "	  	              float differenceSquared = dot(difference, difference);\n",
            "	  	              float distance = sqrt(differenceSquared);\n",
            "	  	              gravitySum[i] += difference / distance / differenceSquared;\n",
            "	  	          }\n",
            "	  	      }\n",
            "	  	  }\n",
            "	  	  barrier();\n",
            "	  	  for(uint i = 0u; i < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * task + i < COUNT; i++){\n",
            "	  	      outBuffer.chunks[task].particles[i].velocity += gravitySum[i];\n",
            "	  	      outBuffer.chunks[task].particles[i].position += outBuffer.chunks[task].particles[i].velocity;\n",
            "	  	  }\n",
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

    void simulate(const int &iterations, bool pushDataToGPU,
                  bool retrieveDataFromGPU);

    ComputationOptions getComputationOption();

    void pushData2GPU();

private:

    float dt;

    void simulateOnCPU();

    void simulateOnGPU(const int &iterations, bool pushDataToGPU,
                       bool retrieveDataFromGPU);

    float getRandomFloat(float x);

    bool seed();

};


#endif //LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
