//
// Created by Immanuel Bierer on 1/6/2024.
//

#include "SimpleNBodySimulation.h"

void SimpleNBodySimulation::initialize(const ComputationOptions& computationOption) {
    this->computationOption = computationOption;
    data = (SimpleNBodySimulationData*)malloc(sizeof(SimpleNBodySimulationData));
    t = 0.0;
    dt = 1.0f;
    seed();
    computeShader.gComputeProgram = View::createComputeShaderProgram(View::stringArrayToString(computeShaderCode, 1000).c_str());
    glGenBuffers(1, &computeShader.gVBO);
}

void SimpleNBodySimulation::simulateOnCPU(){
    if(t <= 0.0f) {
        return;
    }
    vec3 gravitySum[NUM_CACHE_CHUNKS][PARTICLES_PER_CHUNK];
    for (uint task = 0u; task < NUM_CACHE_CHUNKS; task++) {
        for (uint i = 0u; i < PARTICLES_PER_CHUNK; i++) {
            gravitySum[task][i] = vec3(0.0f, 0.0f, 0.0f);
            for (uint j = 0u; j < NUM_CACHE_CHUNKS; j++) {
                for (uint k = 0u; k < PARTICLES_PER_CHUNK; k++) {
                    if (PARTICLES_PER_CHUNK * j + k == PARTICLES_PER_CHUNK * task + i)
                        continue;
                    vec3 difference = data->chunks[j].particles[k].position -
                                      data->chunks[task].particles[i].position;
                    float differenceSquared = dot(difference, difference);
                    float distance = sqrt(differenceSquared);
                    gravitySum[task][i] += difference / distance / differenceSquared;
                }
            }
        }
    }
    for (uint task = 0; task < NUM_CACHE_CHUNKS; task++) {
        for (uint i = 0u; i < PARTICLES_PER_CHUNK; i++) {
            data->chunks[task].particles[i].velocity += gravitySum[task][i];
            data->chunks[task].particles[i].position += data->chunks[task].particles[i].velocity;
        }
    }
}

void SimpleNBodySimulation::simulate(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU) {
    switch(computationOption){
        case CPU:
            for(int i = 0; i < iterations; i++) {
                simulateOnCPU();
            }
            break;
        case GPU:
            simulateOnGPU(iterations, pushDataToGPU, retrieveDataFromGPU);
            break;
    }
    t += dt;
}

void SimpleNBodySimulation::simulateOnGPU(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU) {
    if(pushDataToGPU || !pushed){
        pushData2GPU();
        pushed = true;
    }
    // Bind the compute program
    glUseProgram(computeShader.gComputeProgram);
    // Push uniform and SSBO data to GPU
    glUniform1f(glGetUniformLocation(computeShader.gComputeProgram, "t"), t);
    for(int i = 0; i < iterations && t > 0.0; i++) {
        // Launch work group
        glDispatchCompute(1, 1, 1);
        // Define the end of the ongoing GPU computation as the barrier after which the CPU code may continue to execute
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
    if(retrieveDataFromGPU) {
        // Bind buffer object simulation.computeShader.gVBO to target GL_ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, computeShader.gVBO);
        // Map a section of buffer object simulation.computeShader.gVBO's data store
        data = (SimpleNBodySimulationData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SimpleNBodySimulationData), GL_MAP_READ_BIT);
        // Unmap buffer object simulation.computeShader.gVBO's data store
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SimpleNBodySimulation::pushData2GPU(){
    // Bind buffer object simulation.computeShader.gVBO to indexed buffer target GL_SHADER_STORAGE_BUFFER at index simulation.computeShader.gIndexBufferBinding
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, computeShader.gIndexBufferBinding, computeShader.gVBO);
    // Create and initialize data store for buffer object simulation.computeShader.gVBO with a size of sizeof(LinearithmicSimulationData) and dynamically copy data to it from pointer location verts
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SimpleNBodySimulationData), data, GL_DYNAMIC_COPY);// STREAM = infrequent use and changes, STATIC = frequent use and infrequent changes, DYNAMIC = frequent use and frequent changes
}

float SimpleNBodySimulation::getRandomFloat(float x) {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return random * x;
}

bool SimpleNBodySimulation::seed() {
    switch(computationOption){
        case CPU:
            /*for(int i = 0; i < COUNT; i++){
                data->stars[i].position = vec3(getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f);
                data->stars[i].velocity = vec3(0.0f);
            }*/
            for(int i = 0; i < NUM_CACHE_CHUNKS; i++){
                for(int j = 0; j < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * i + j < COUNT; j++){
                    data->chunks[i].particles[j].position = vec3(getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f);
                    data->chunks[i].particles[j].velocity = vec3(0.0f);
                }
            }
            break;
        case GPU:
            for(int i = 0; i < NUM_CACHE_CHUNKS; i++){
                for(int j = 0; j < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * i + j < COUNT; j++){
                    data->chunks[i].particles[j].position = vec3(getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f, getRandomFloat(100.0f) - 50.0f);
                    data->chunks[i].particles[j].velocity = vec3(0.0f);
                }
            }
            break;
    }
    return true;
}

Computation::ComputationOptions SimpleNBodySimulation::getComputationOption() {
    return computationOption;
}
