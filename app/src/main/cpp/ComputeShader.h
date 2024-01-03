//
// Created by Immanuel Bierer on 1/2/2024.
//

#ifndef LIVEWALLPAPER05_COMPUTESHADER_H
#define LIVEWALLPAPER05_COMPUTESHADER_H


#define numCacheChunks 1024
#define starsPerChunk 8
//#define starsPerChunk 13
//#define starsPerChunk 21
#define COUNT 8192
//#define COUNT 13312
//#define COUNT 21504

float getRandomFloat(float x) {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return random * x;
}

const int PORTRAIT = 0;
const int LANDSCAPE = 1;
int orientation = PORTRAIT;

struct Star {
    vec3 position;
    vec3 velocity;
};

struct cacheChunk{
    Star stars[starsPerChunk]; // 16 * 24 bytes = 3 * 128 bytes
    //float padding[16 + 0];
};

struct __attribute__((aligned(128))) SimulationData {
    union{
        Star stars[COUNT];
        cacheChunk chunks[numCacheChunks];
    };
};

SimulationData* data;

class Simulation {
public:

    double t;
    GLuint _t;
    static const int CPU_OPTION = 0;
    static const int GPU_OPTION = 1;

    Simulation(){

    }

    ~Simulation(){
        //free(data);
        //delete data;
        // Delete buffer object graphComputeShader.gVBO
        glDeleteBuffers(1, &computeShader.gVBO);
    }

    void initialize(int simulationOption){
        this->simulationOption = simulationOption;
        data = (SimulationData*)malloc(sizeof(SimulationData));
        //data = new SimulationData;
        t = 0.0;
        dt = 1.0f;
        seed();
    }

    void simulate(bool pushDataToGPU){
        switch(simulationOption){
            case CPU_OPTION:
                simulateOnCPU();
                break;
            case GPU_OPTION:
                simulateOnGPU(pushDataToGPU);
                break;
        }
        t += dt;
    }

    /*vec3 getPosition(int i){
        switch(simulationOption){
        case CPU_OPTION:
            return data->stars[i].position;
            break;
        case GPU_OPTION:
            return data->chunks[i / starsPerChunk].stars[i % starsPerChunk].position;
            break;
        }
    }

    void setPosition(int i, vec3 position){
        switch(simulationOption){
        case CPU_OPTION:
            data->stars[i].position = position;
            break;
        case GPU_OPTION:
            data->chunks[i / starsPerChunk].stars[i % starsPerChunk].position = position;
            break;
        }
    }

    vec3 getVelocity(int i){
        switch(simulationOption){
        case CPU_OPTION:
            return data->stars[i].velocity;
            break;
        case GPU_OPTION:
            return data->chunks[i / starsPerChunk].stars[i % starsPerChunk].velocity;
            break;
        }
    }

    void setVelocity(int i, vec3 velocity){
        switch(simulationOption){
        case CPU_OPTION:
            data->stars[i].velocity = velocity;
            break;
        case GPU_OPTION:
            data->chunks[i / starsPerChunk].stars[i % starsPerChunk].position = velocity;
            break;
        }
    }*/

    int simulationOption; // Should be private
private:

    float dt;
    bool computeShaderGenerated = false;

    ComputeShader computeShader;

    void simulateOnCPU(){
        vec3 gravitySum[COUNT];
        for(uint i = 0u; i < COUNT; i++){
            gravitySum[i] = vec3(0.0f);
            for(uint j = 0u; j < COUNT; j++){
                if(j == i)
                    continue;
                vec3 difference = data->stars[j].position - data->stars[i].position;
                float differenceSquared = dot(difference, difference);
                float distance = sqrt(differenceSquared);
                gravitySum[i] += difference / distance / differenceSquared;
            }
        }

        for(uint i = 0u; i < COUNT; i++){
            data->stars[i].velocity += gravitySum[i];
            data->stars[i].position += data->stars[i].velocity;
        }
    }

    void simulateOnGPU(bool pushDataToGPU){

        bool pushed = false;

        if(!computeShaderGenerated){

            // https://community.arm.com/developer/tools-software/graphics/b/blog/posts/get-started-with-compute-shaders
            char* computeShaderCode =
                    "#version 320 es\n"
            "const uint starsPerChunk = " STRV(starsPerChunk) "u;\n"
                                                              "const uint numCacheChunks = uint(" STRV(numCacheChunks) ");\n"
                                                                                                                       "const uint COUNT = " STRV(COUNT) "u;\n"
                                                                                                                                                         R"(struct Star{
					vec3 position;
					vec3 velocity;
				};
				struct cacheChunk{
					Star stars[starsPerChunk];
					//float padding[16 + 0];// Profile this padding
				};
				struct SimulationData{
					cacheChunk chunks[numCacheChunks];
				};
				layout(packed, binding = 0) buffer destBuffer{
					SimulationData data;
				} outBuffer;
				uniform float t;
				uint task;
				layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
				void main(){
					task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;
					if(task < numCacheChunks && t > 0.0f){
						vec3 gravitySum[starsPerChunk];
						for(uint i = 0u; i < starsPerChunk && starsPerChunk * task + i < COUNT; i++){
							gravitySum[i] = vec3(0.0f, 0.0f, 0.0f);
							for(uint j = 0u; j < numCacheChunks; j++){
								for(uint k = 0u; k < starsPerChunk && starsPerChunk * j + k < COUNT; k++){
									if(starsPerChunk * j + k == starsPerChunk * task + i)
										continue;
									vec3 difference = outBuffer.data.chunks[j].stars[k].position - outBuffer.data.chunks[task].stars[i].position;
									float differenceSquared = dot(difference, difference);
									float distance = sqrt(differenceSquared);
									gravitySum[i] += difference / distance / differenceSquared;
								}
							}
						}
						barrier();
						for(uint i = 0u; i < starsPerChunk && starsPerChunk * task + i < COUNT; i++){
							outBuffer.data.chunks[task].stars[i].velocity += gravitySum[i];
							outBuffer.data.chunks[task].stars[i].position += outBuffer.data.chunks[task].stars[i].velocity;
						}
					}
				}
			)";

            computeShader.gComputeProgram = computeShader.generateComputeShader(computeShaderCode);
            _t = glGetUniformLocation(computeShader.gComputeProgram, "t");
            glGenBuffers(1, &computeShader.gVBO);
            computeShaderGenerated = true;
            pushData2GPU();
            pushed = true;
        }

        // Bind the compute program
        glUseProgram(computeShader.gComputeProgram);
        // Push uniform and SSBO data to GPU
        glUniform1f(_t, t);
        if(pushDataToGPU && !pushed){
            pushData2GPU();
            pushed = true; // This instruction is not needed
        }
        // Launch work group
        glDispatchCompute(1, 1, 1);
        // Define the end of the ongoing GPU computation as the barrier after which the CPU code may continue to execute
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        // Bind buffer object simulation.computeShader.gVBO to target GL_ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, computeShader.gVBO);
        // Map a section of buffer object simulation.computeShader.gVBO's data store
        data = (SimulationData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SimulationData), GL_MAP_READ_BIT);
        // Unmap buffer object simulation.computeShader.gVBO's data store
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void pushData2GPU(){
        // Bind buffer object simulation.computeShader.gVBO to indexed buffer target GL_SHADER_STORAGE_BUFFER at index simulation.computeShader.gIndexBufferBinding
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, computeShader.gIndexBufferBinding, computeShader.gVBO);
        // Create and initialize data store for buffer object simulation.computeShader.gVBO with a size of sizeof(SimulationData) and dynamically copy data to it from pointer location verts
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SimulationData), data, GL_DYNAMIC_COPY);// STREAM = infrequent use and changes, STATIC = frequent use and infrequent changes, DYNAMIC = frequent use and frequent changes
    }

    bool seed() {
        switch(simulationOption){
            case CPU_OPTION:
                for(int i = 0; i < COUNT; i++){
                    data->stars[i].position = vec3(getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f);
                    data->stars[i].velocity = vec3(0.0f);
                }
                break;
            case GPU_OPTION:
                for(int i = 0; i < numCacheChunks; i++){
                    for(int j = 0; j < starsPerChunk && starsPerChunk * i + j < COUNT; j++){
                        data->chunks[i].stars[j].position = vec3(getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f);
                        data->chunks[i].stars[j].velocity = vec3(0.0f);
                    }
                }
                break;
        }
        return true;
    }
};
Simulation simulation;


#endif //LIVEWALLPAPER05_COMPUTESHADER_H
