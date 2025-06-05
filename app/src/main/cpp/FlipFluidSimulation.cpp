//
// Created by Immanuel Bierer on 5/4/2025.
//

#include "FlipFluidSimulation.h"

using std::min;
using std::max;

void FlipFluidSimulation::initialize(const ComputationOptions& computationOption) {
    this->computationOption = computationOption;
    data = (FlipFluidSimulationData*)malloc(sizeof(FlipFluidSimulationData));
    t = 0.0;
    seed();
    computeShader.gComputeProgram = View::createComputeShaderProgram(View::stringArrayToString(computeShaderCode, 1000).c_str());
    glGenBuffers(1, &computeShader.gVBO);
}

void FlipFluidSimulation::simulateOnCPU(){
    if(t <= 0.0f) {
        return;
    }
    //vec3 gravitySum[NUM_CACHE_CHUNKS][PARTICLES_PER_CHUNK];
    //for (uint task = 0u; task < NUM_CACHE_CHUNKS; task++) {
    //    for (uint i = 0u; i < PARTICLES_PER_CHUNK; i++) {
    //        gravitySum[task][i] = vec3(0.0f, 0.0f, 0.0f);
    //        for (uint j = 0u; j < NUM_CACHE_CHUNKS; j++) {
    //            for (uint k = 0u; k < PARTICLES_PER_CHUNK; k++) {
    //                if (PARTICLES_PER_CHUNK * j + k == PARTICLES_PER_CHUNK * task + i)
    //                    continue;
    //                vec3 difference = data->chunks[j].particles[k].position -
    //                                  data->chunks[task].particles[i].position;
    //                float differenceSquared = dot(difference, difference);
    //                float distance = sqrt(differenceSquared);
    //                gravitySum[task][i] += difference / distance / differenceSquared;
    //            }
    //        }
    //    }
    //}
    //for (uint task = 0; task < NUM_CACHE_CHUNKS; task++) {
    //    for (uint i = 0u; i < PARTICLES_PER_CHUNK; i++) {
    //        data->chunks[task].particles[i].velocity += gravitySum[task][i];
    //        data->chunks[task].particles[i].position += data->chunks[task].particles[i].velocity;
    //    }
    //}
}

void FlipFluidSimulation::simulate(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU, const vec3& forceVector) {
    switch(computationOption){
        case CPU:
            for(int i = 0; i < iterations; i++) {
                simulateOnCPU();
            }
            break;
        case GPU:
            simulateOnGPU(iterations, pushDataToGPU, retrieveDataFromGPU, forceVector);
            break;
    }
    t += dt;
}

void FlipFluidSimulation::simulateOnGPU(const int &iterations, bool pushDataToGPU, bool retrieveDataFromGPU, const vec3& forceVector) {
    if(pushDataToGPU || !pushed){
        pushData2GPU();
        pushed = true;
    }
    // Bind the compute program
    glUseProgram(computeShader.gComputeProgram);
    // Push uniforms and SSBO data to GPU
    glUniform1f(glGetUniformLocation(computeShader.gComputeProgram, "t"), t);
    glUniform3fv(glGetUniformLocation(computeShader.gComputeProgram, "acceleration"), 1, forceVector.v);
    for(int i = 0; i < iterations && t > 0.0; i++) {
        // Launch work group
        glDispatchCompute(NUM_GROUPS_X, NUM_GROUPS_Y, NUM_GROUPS_Z);
        // Define the end of the ongoing GPU computation as the barrier after which the CPU code may continue to execute
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
    if(retrieveDataFromGPU) {
        // Bind buffer object simulation.computeShader.gVBO to target GL_ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, computeShader.gVBO);
        // Map a section of buffer object simulation.computeShader.gVBO's data store
        data = (FlipFluidSimulationData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(FlipFluidSimulationData), GL_MAP_READ_BIT);
        // Unmap buffer object simulation.computeShader.gVBO's data store
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        for(int i = 0; i < 4096; i++) {
            //ALOGI("logValues[%d] = (%d, %d, %d), (%d, %d, %d), %d", i, data->logValues[i][0], data->logValues[i][1], data->logValues[i][2], data->logValues[i][3], data->logValues[i][4], data->logValues[i][5], data->logValues[i][6], data->logValues[i][7]);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FlipFluidSimulation::pushData2GPU(){
    // Bind buffer object simulation.computeShader.gVBO to indexed buffer target GL_SHADER_STORAGE_BUFFER at index simulation.computeShader.gIndexBufferBinding
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, computeShader.gIndexBufferBinding, computeShader.gVBO);
    // Create and initialize data store for buffer object simulation.computeShader.gVBO with a size of sizeof(LinearithmicSimulationData) and dynamically copy data to it from pointer location verts
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(FlipFluidSimulationData), data, GL_DYNAMIC_COPY);// STREAM = infrequent use and changes, STATIC = frequent use and infrequent changes, DYNAMIC = frequent use and frequent changes
}

float FlipFluidSimulation::getRandomFloat(float x) {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return random * x;
}

bool FlipFluidSimulation::seed() {
    ALOGI("fNumCells = %d\n", fNumCells);
    ALOGI("sizeof(data->fCells) = %d\n", fNumCells * sizeof(fCell));
    ALOGI("pNumCells = %d\n", pNumCells);
    ALOGI("sizeof(data->pCells) = %d\n", (pNumCells + 1) * sizeof(pCell));
    ALOGI("maxParticles = %d\n", maxParticles);
    ALOGI("sizeof(data->particles) = %d\n", maxParticles * sizeof(ParticleInfo));
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            for (int k = 0; k < numZ; k++) {
                data->particles[(i * numY + j) * numZ + k].position = vec3(
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
                data->fCells[(i * fNumY + j) * fNumZ + k].s = cellDensity;
            }
        }
    }
    return true;
}

Computation::ComputationOptions FlipFluidSimulation::getComputationOption() {
    return computationOption;
}
