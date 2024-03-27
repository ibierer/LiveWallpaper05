//
// Created by camer on 2/29/2024.
//

#include "LinearithmicNBodySimulation.h"

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

void LinearithmicNBodySimulation::integrate() {
    float deltaTime = 0.1f;
    for (uint i = 0u; i < COUNT; i++) {
        vec3 acceleration = data->stars[i].initialForce / data->stars[i].mass;
        //update positions
        data->stars[i].projectedPosition = data->stars[i].position + (data->stars[i].velocity + 0.5f * acceleration * deltaTime) * deltaTime;
    }
}

void LinearithmicNBodySimulation::finalIntegrate() {
    float deltaTime = 0.1f;
    for (uint i = 0u; i < COUNT; i++) {
        vec3 avgForce = 0.5f * (data->stars[i].initialForce + data->stars[i].finalForce);
        vec3 acceleration = avgForce / data->stars[i].mass;
        //update positions
        data->stars[i].position += (data->stars[i].velocity + 0.5f * acceleration * deltaTime) * deltaTime;
        //update velocity
        data->stars[i].velocity += acceleration * deltaTime;
    }
}

vec4 LinearithmicNBodySimulation::conquerVolume(const vector<int> &ids, Node *node) {
    // Base case
    if (ids.size() == 1) {
        int id = ids.at(0);
        return vec4(data->stars[id].position.x, data->stars[id].position.y,
                    data->stars[id].position.z, data->stars[id].mass);
    }

    float mass = 0.0f;
    vec3 numerator = vec3(0.0f);
    float denominator = 0.0f;
    float halfSize = 0.5f * node->size;
    float quarterSize = 0.25f * node->size;
    vector<int> childIDs[8];

    // Populate 8 octants
    for (int i = 0; i < ids.size(); i++) {
        vec3 p = data->stars[ids.at(i)].position;
        int combo = getCombo(p.x > node->center.x, p.y > node->center.y, p.z > node->center.z);
        childIDs[combo].push_back(ids[i]);
    }
    // Iterate over 8 octants
    for (int i = 0; i < 8; i++) {
        if (childIDs[i].size() > 0) {
            std::tuple<bool, bool, bool> inverseCombo = getInverseCombo(i);
            node->children[i] = new Node{
                    childIDs[i].size() == 1,
                    node,
                    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
                    vec4(0.0f),
                    vec3(
                            node->center.x + (std::get<0>(inverseCombo) ? quarterSize : -quarterSize),
                            node->center.y + (std::get<1>(inverseCombo) ? quarterSize : -quarterSize),
                            node->center.z + (std::get<2>(inverseCombo) ? quarterSize : -quarterSize)
                    ),
                    halfSize
            };
            node->children[i]->centerOfGravity = conquerVolume(childIDs[i], node->children[i]);
            mass += node->children[i]->centerOfGravity.w;
            numerator += node->children[i]->centerOfGravity.w * node->children[i]->centerOfGravity.xyz;
            denominator += node->children[i]->centerOfGravity.w;
            if (childIDs[i].size() == 1) {
                data->stars[childIDs[i].at(0)].leaf = node->children[i];
            }
        }
    }
    vec3 quotient = numerator / denominator;
    return vec4(quotient.x, quotient.y, quotient.z, mass);
}

vec4 LinearithmicNBodySimulation::conquerOnceMore(const vector<int> &ids, Node *node) {
    // Base case
    if (ids.size() == 1) {
        int id = ids.at(0);
        return vec4(data->stars[id].projectedPosition.x, data->stars[id].projectedPosition.y,
                    data->stars[id].projectedPosition.z, data->stars[id].mass);
    }

    float mass = 0.0f;
    vec3 numerator = vec3(0.0f);
    float denominator = 0.0f;
    float halfSize = 0.5f * node->size;
    float quarterSize = 0.25f * node->size;
    vector<int> childIDs[8];

    // Populate 8 octants
    for (int i = 0; i < ids.size(); i++) {
        vec3 p = data->stars[ids.at(i)].projectedPosition;
        int combo = getCombo(p.x > node->center.x, p.y > node->center.y, p.z > node->center.z);
        childIDs[combo].push_back(ids[i]);
    }

    // Iterate over 8 octants
    for (int i = 0; i < 8; i++) {
        if (childIDs[i].size() > 0) {
            std::tuple<bool, bool, bool> inverseCombo = getInverseCombo(i);
            node->children[i] = new Node{
                    childIDs[i].size() == 1,
                    node,
                    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
                    vec4(0.0f),
                    vec3(
                            node->center.x + (std::get<0>(inverseCombo) ? quarterSize : -quarterSize),
                            node->center.y + (std::get<1>(inverseCombo) ? quarterSize : -quarterSize),
                            node->center.z + (std::get<2>(inverseCombo) ? quarterSize : -quarterSize)
                    ),
                    halfSize
            };
            node->children[i]->centerOfGravity = conquerOnceMore(childIDs[i], node->children[i]);
            mass += node->children[i]->centerOfGravity.w;
            numerator += node->children[i]->centerOfGravity.w * node->children[i]->centerOfGravity.xyz;
            denominator += node->children[i]->centerOfGravity.w;
            if (childIDs[i].size() == 1) {
                data->stars[childIDs[i].at(0)].leaf = node->children[i];
            }
        }
    }
    vec3 quotient = numerator / denominator;
    return vec4(quotient.x, quotient.y, quotient.z, mass);
}

vec3 LinearithmicNBodySimulation::addForces(Node *node, int index) {
    float theta = 0.7f;
    // BASE CASE
    if (node->isLeaf || node->size / distance(data->stars[index].position, node->centerOfGravity.xyz) < theta) {
        if (node == data->stars[index].leaf) {
            return vec3(0.0f);
        }
        vec3 delta = node->centerOfGravity.xyz - data->stars[index].position;
        float distSquared = dot(delta, delta);
        float dist = sqrt(distSquared);
        return delta / (dist * distSquared) * node->centerOfGravity.w;
    } else {
        vec3 sumForces = vec3(0.0f);
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                sumForces += addForces(node->children[i], index);
            }
        }
        return sumForces;
    }
}

vec3 LinearithmicNBodySimulation::addForcesFinal(Node *node, int index) {
    float theta = 0.7f;
    // BASE CASE
    if (node->isLeaf || node->size / distance(data->stars[index].projectedPosition, node->centerOfGravity.xyz) < theta) {
        if (node == data->stars[index].leaf) {
            return vec3(0.0f);
        }
        vec3 delta = node->centerOfGravity.xyz - data->stars[index].projectedPosition;
        float distSquared = dot(delta, delta);
        float dist = sqrt(distSquared);
        return delta / (dist * distSquared) * node->centerOfGravity.w;
    } else {
        vec3 sumForces = vec3(0.0f);
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                sumForces += addForcesFinal(node->children[i], index);
            }
        }
        return sumForces;
    }
}

void LinearithmicNBodySimulation::computeForcesOnCPULinearithmic() {
    // Delete the octree if it already exists
    if (root != nullptr) {
        delete root;
    }
    // Compute size of root node
    float radius = 0.5f;
    for (int j = 0; j < COUNT; j++) {
        while (abs(data->stars[j].position.x) >= radius ||
               abs(data->stars[j].position.y) >= radius ||
               abs(data->stars[j].position.z) >= radius) {
            radius *= 2.0f;
        }
    }
    // Initialize root
    root = new Node{
            false,
            nullptr,
            {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
            vec4(0.0f),
            vec3(0.0f),
            2.0f * radius
    };
    if (!ids.empty()){
        ids.clear();
    }
    // Pack ids into a vector
    for (int j = 0; j < COUNT; j++) {
        ids.push_back(j);
    }

    // Use conquerVolume to recursively define octree
    root->centerOfGravity = conquerVolume(ids, root);
    // Sum the forces for each particle
    for (int i = 0; i < COUNT; i++) {
        data->stars[i].initialForce = addForces(root, i);
    }
}

void LinearithmicNBodySimulation::computeForcesOnCPULinearithmicFinal() {
    // Delete the octree if it already exists
    if (root != nullptr) {
        delete root;
    }
    // Compute size of root node
    float radius = 0.5f;
    for (int j = 0; j < COUNT; j++) {
        while (abs(data->stars[j].projectedPosition.x) >= radius ||
               abs(data->stars[j].projectedPosition.y) >= radius ||
               abs(data->stars[j].projectedPosition.z) >= radius) {
            radius *= 2.0f;
        }
    }
    // Initialize root
    root = new Node{
            false,
            nullptr,
            {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
            vec4(0.0f),
            vec3(0.0f),
            2.0f * radius
    };

    // Use conquerVolume to recursively define octree
    root->centerOfGravity = conquerOnceMore(ids, root);
    // Sum the forces for each particle
    for (int i = 0; i < COUNT; i++) {
        data->stars[i].finalForce = addForcesFinal(root, i);
    }
}

void LinearithmicNBodySimulation::simulate(const int &iterations, bool pushDataToGPU,
                                           bool retrieveDataFromGPU) {
    switch (computationOption) {
        case CPU:
            for (int i = 0; i < iterations; i++) {
                computeForcesOnCPULinearithmic();
                integrate();
                computeForcesOnCPULinearithmicFinal();
                finalIntegrate();
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

int LinearithmicNBodySimulation::getCombo(bool xIsPos, bool yIsPos, bool zIsPos) {
    return xIsPos * 1 + yIsPos * 2 + zIsPos * 4;
}

std::tuple<bool, bool, bool> LinearithmicNBodySimulation::getInverseCombo(int combo) {
    //return std::make_tuple(combo & 1, combo & 2, combo & 4);
    return std::make_tuple((combo & 1) != 0, (combo & 2) != 0, (combo & 4) != 0);
}