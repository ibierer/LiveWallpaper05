//
// Created by camer on 2/29/2024.
//

#include "LinearithmicNBodySimulation.h"
#include "View.h"
#include "gl3stub.h"

void LinearithmicNBodySimulation::initialize(const ComputationOptions &computationOption) {
    this->computationOption = computationOption;
    data = (LinearithmicNBodySimulationData *) malloc(sizeof(LinearithmicNBodySimulationData));
    t = 0.0;
    dt = 1.0f;
    seed();
    computeShader.gComputeProgram = View::createComputeShaderProgram(
            View::stringArrayToString(computeShaderCode, 1000).c_str());
    glGenBuffers(1, &computeShader.gVBO);
}

void LinearithmicNBodySimulation::simulateOnCPU() {
    float gravitationalConstant = 6.674e-11f;
    if (t <= 0.0f) {
        return;
    }
    // Sum forces
    vec3 gravitySum[COUNT];
    for (uint i = 0u; i < COUNT; i++) {
        // set grav sum to
        gravitySum[i] = vec3(0.0f);
        for (uint j = 0u; j < COUNT; j++) {
            if (i == j) {
                continue;
            }
            vec3 delta = data->stars[j].position - data->stars[i].position;
            float distSquared = dot(delta, delta);
            float dist = sqrt(distSquared);
            gravitySum[i] += delta / (dist * distSquared) * data->stars[j].mass;
        }
        gravitySum[i] *= data->stars[i].mass;
    }
    // INTEGRATION --------------------------------------------------
    float deltaTime = 0.1f;
    for (uint i = 0u; i < COUNT; i++) {
        //update positions
        data->stars[i].position += (data->stars[i].velocity + 0.5f * gravitySum[i] * deltaTime) * deltaTime;
        //update velocity
        data->stars[i].velocity += gravitySum[i] * deltaTime;
    }
}

struct Node {
    Node* children[8];
};
// for(int i = 0; i < 8; i++){children[i] = null}
vec4 LinearithmicNBodySimulation::conquerVolume(vector<int> ids){

    for(int i = 0; i < ids.size(); i++){
        vec3 p = data->stars[ids.at(i)].position;
        int combo = (p.x>0.0f)*1+(p.y>0.0f)*2+(p.z>0.0f)*4;

    }

}

void LinearithmicNBodySimulation::simulate(const int &iterations, bool pushDataToGPU,
                                           bool retrieveDataFromGPU) {
    switch (computationOption) {
        case CPU:
            for (int i = 0; i < iterations; i++) {
                simulateOnCPU();
            }
            break;
        case GPU:
            simulateOnGPU(iterations, pushDataToGPU, retrieveDataFromGPU);
            break;
    }
    t += dt;
}

void LinearithmicNBodySimulation::simulateOnGPU(const int &iterations, bool pushDataToGPU,
                                                bool retrieveDataFromGPU) {/*
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void LinearithmicNBodySimulation::pushData2GPU() {
    // Bind buffer object simulation.computeShader.gVBO to indexed buffer target GL_SHADER_STORAGE_BUFFER at index simulation.computeShader.gIndexBufferBinding
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, computeShader.gIndexBufferBinding,
                     computeShader.gVBO);
    // Create and initialize data store for buffer object simulation.computeShader.gVBO with a size of sizeof(LinearithmicSimulationData) and dynamically copy data to it from pointer location verts
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LinearithmicNBodySimulationData), data,
                 GL_DYNAMIC_COPY);// STREAM = infrequent use and changes, STATIC = frequent use and infrequent changes, DYNAMIC = frequent use and frequent changes
}

float LinearithmicNBodySimulation::getRandomFloat(float x) {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return random * x;
}

bool LinearithmicNBodySimulation::seed() {
    switch (computationOption) {
        case CPU:
            for (int i = 0; i < COUNT; i++) {
                data->stars[i].position = vec3(getRandomFloat(100.0f) - 50.0f,
                                              getRandomFloat(100.0f) - 50.0f,
                                              getRandomFloat(100.0f) - 50.0f);
                data->stars[i].velocity = vec3(0.0f);
                data->stars[i].mass = 1.0f;
            }
            /*for (int i = 0; i < NUM_CACHE_CHUNKS; i++) {
                for (int j = 0;
                     j < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * i + j < COUNT; j++) {
                    data->chunks[i].particles[j].position = vec3(getRandomFloat(100.0f) - 50.0f,
                                                                 getRandomFloat(100.0f) - 50.0f,
                                                                 getRandomFloat(100.0f) - 50.0f);
                    data->chunks[i].particles[j].velocity = vec3(0.0f);
                }
            }*/
            break;
        case GPU:
            for (int i = 0; i < NUM_CACHE_CHUNKS; i++) {
                for (int j = 0;
                     j < PARTICLES_PER_CHUNK && PARTICLES_PER_CHUNK * i + j < COUNT; j++) {
                    data->chunks[i].particles[j].position = vec3(getRandomFloat(100.0f) - 50.0f,
                                                                 getRandomFloat(100.0f) - 50.0f,
                                                                 getRandomFloat(100.0f) - 50.0f);
                    data->chunks[i].particles[j].velocity = vec3(0.0f);
                }
            }
            break;
    }
    return true;
}

Computation::ComputationOptions LinearithmicNBodySimulation::getComputationOption() {
    return computationOption;
}