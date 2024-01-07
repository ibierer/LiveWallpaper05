//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
#define LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H


class SimpleNBodySimulation : public Simulation {
public:

#define numCacheChunks 1024
#define starsPerChunk 10
#define COUNT 10240

    struct cacheChunk { // 256 bytes
        Simulation::Particle stars[starsPerChunk]; // 240 bytes
        float padding[4]; // Profile this padding
    };

    struct __attribute__((aligned(256))) SimpleNBodySimulationData { // 2^18 (256K) bytes
        union {
            Particle stars[COUNT]; // CPU computation data
            cacheChunk chunks[numCacheChunks]; // GPU computation data
        };
    };

    // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
    string computeShaderCode[1000] = {
            "#version 320 es\n",
            "const uint starsPerChunk = " STRV(starsPerChunk) "u;",
            "const uint numCacheChunks = uint(" STRV(numCacheChunks) ");\n",
            "const uint COUNT = " STRV(COUNT) "u;\n",
            "struct Particle {\n",
            "    vec3 position;\n",
            "    vec3 velocity;\n",
            "};\n",
            "struct cacheChunk{\n",
            "    Particle stars[starsPerChunk];\n",
            "    float padding[4]; // Profile this padding\n",
            "};\n",
            "layout(packed, binding = 0) buffer destBuffer{\n",
            "	  cacheChunk chunks[numCacheChunks];\n",
            "} outBuffer;\n",
            "uniform float t;\n",
            "uint task;\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "void main(){\n",
            "	  task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
            "	  if(task < numCacheChunks && t > 0.0f){\n",
            "	  	vec3 gravitySum[starsPerChunk];\n",
            "	  	for(uint type = 0u; type < starsPerChunk && starsPerChunk * task + type < COUNT; type++){\n",
            "	  		gravitySum[type] = vec3(0.0f, 0.0f, 0.0f);\n",
            "	  		for(uint j = 0u; j < numCacheChunks; j++){\n",
            "	  			for(uint k = 0u; k < starsPerChunk && starsPerChunk * j + k < COUNT; k++){\n",
            "	  				if(starsPerChunk * j + k == starsPerChunk * task + type)\n",
            "	  					continue;\n",
            "	  				vec3 difference = outBuffer.chunks[j].stars[k].position - outBuffer.chunks[task].stars[type].position;\n",
            "	  				float differenceSquared = dot(difference, difference);\n",
            "	  				float distance = sqrt(differenceSquared);\n",
            "	  				gravitySum[type] += difference / distance / differenceSquared;\n",
            "	  			}\n",
            "	  		}\n",
            "	  	}\n",
            "	  	barrier();\n",
            "	  	for(uint type = 0u; type < starsPerChunk && starsPerChunk * task + type < COUNT; type++){\n",
            "	  		outBuffer.chunks[task].stars[type].velocity += gravitySum[type];\n",
            "	  		outBuffer.chunks[task].stars[type].position += outBuffer.chunks[task].stars[type].velocity;\n",
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

    void initialize(int computationOption);

    void simulate(bool pushDataToGPU);

private:

    float dt;

    void simulateOnCPU();

    void simulateOnGPU(bool pushDataToGPU);

    void pushData2GPU();

    float getRandomFloat(float x);

    bool seed();

};


#endif //LIVEWALLPAPER05_SIMPLENBODYSIMULATION_H
