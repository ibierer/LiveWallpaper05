//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;












































/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <EGL/egl.h>

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

class Simulation{
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
            string computeShaderCode[1000] = {
                    "#version 320 es\n",
                    "const uint starsPerChunk = " STRV(starsPerChunk) "u;",
                    "const uint numCacheChunks = uint(" STRV(numCacheChunks) ");\n",
                    "const uint COUNT = " STRV(COUNT) "u;\n",
                    "struct Star{\n",
                    "	vec3 position;\n",
                    "	vec3 velocity;\n",
                    "};\n",
                    "struct cacheChunk{\n",
                    "	Star stars[starsPerChunk];\n",
                    "	//float padding[16 + 0];// Profile this padding\n",
                    "};\n",
                    "struct SimulationData{\n",
                    "	cacheChunk chunks[numCacheChunks];\n",
                    "};\n",
                    "layout(packed, binding = 0) buffer destBuffer{\n",
                    "	SimulationData data;\n",
                    "} outBuffer;\n",
                    "uniform float t;\n",
                    "uint task;\n",
                    "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
                    "void main(){\n",
                    "	task = gl_WorkGroupSize.x * gl_LocalInvocationID.x + gl_LocalInvocationID.y;\n",
                    "	if(task < numCacheChunks && t > 0.0f){\n",
                    "		vec3 gravitySum[starsPerChunk];\n",
                    "		for(uint type = 0u; type < starsPerChunk && starsPerChunk * task + type < COUNT; type++){\n",
                    "			gravitySum[type] = vec3(0.0f, 0.0f, 0.0f);\n",
                    "			for(uint j = 0u; j < numCacheChunks; j++){\n",
                    "				for(uint k = 0u; k < starsPerChunk && starsPerChunk * j + k < COUNT; k++){\n",
                    "					if(starsPerChunk * j + k == starsPerChunk * task + type)\n",
                    "						continue;\n",
                    "					vec3 difference = outBuffer.data.chunks[j].stars[k].position - outBuffer.data.chunks[task].stars[type].position;\n",
                    "					float differenceSquared = dot(difference, difference);\n",
                    "					float distance = sqrt(differenceSquared);\n",
                    "					gravitySum[type] += difference / distance / differenceSquared;\n",
                    "				}\n",
                    "			}\n",
                    "		}\n",
                    "		barrier();\n",
                    "		for(uint type = 0u; type < starsPerChunk && starsPerChunk * task + type < COUNT; type++){\n",
                    "			outBuffer.data.chunks[task].stars[type].velocity += gravitySum[type];\n",
                    "			outBuffer.data.chunks[task].stars[type].position += outBuffer.data.chunks[task].stars[type].velocity;\n",
                    "		}\n",
                    "	}\n",
                    "}"
            };

            computeShader.gComputeProgram = View::createComputeShaderProgram(
                    View::stringArrayToString(computeShaderCode, 1000).c_str());
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

VertexArrayObject cubeVAO;

const string VERTEX_SHADER =
        View::ES_VERSION +
        "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
        "uniform mat4 mvp;\n"
        "out vec4 vColor;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(pos, 1.0);\n"
        "}\n";

const string FRAGMENT_SHADER =
        View::ES_VERSION +
        "precision mediump float;\n"
        "uniform vec4 color;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = color;\n"
        "}\n";

GLuint cubeProgram;
















GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    implicitGrapher = ImplicitGrapher(ivec3(29));
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;

    cubeProgram = View::createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    simulation.initialize(Simulation::GPU_OPTION);
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    ImplicitGrapher::calculateSurfaceOnGPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);
    /*struct chunk {
        bool plusMinus[32];
        ivec3 xyzLineIndex[32];
        PositionXYZNormalXYZ vertices[3 * 32];
        uvec3 indices[3 * 3 * 32];
        int solutionCount;
        int numIndices;
        bool padding[2008];
    };*/
    /*for(int type = 0; type < 1024; type++) {
        for(int j = 0; j < 32; j++) {
            ImplicitGrapher::data->chunks[type].plusMinus[j] = false;
            ImplicitGrapher::data->chunks[type].xyzLineIndex[j] = ivec3(0);
            for(int k = 0; k < 3; k++) {
                ImplicitGrapher::data->chunks[type].vertices[3 * j + k] = PositionXYZNormalXYZ(vec3(0.0f), vec3(0.0f));
                for(int l = 0; l < 3; l++) {
                    ImplicitGrapher::data->chunks[type].indices[9 * j + 3 * k + l] = ivec3(0);
                }
            }
        }
        ImplicitGrapher::data->chunks[type].solutionCount = 0;
        ImplicitGrapher::data->chunks[type].numIndices = 0;
        for(int j = 0; j < sizeof(ImplicitGrapher::chunk::padding); j++) {
            ImplicitGrapher::data->chunks[type].padding[j] = false;
        }
    }*/
    /*struct GPUdata {
        chunk chunks[1024];
        ivec3 sequence[maxEquationLength];
        int constants[maxEquationLength];
        float equationValues[maxEquationLength];
        int valuesCounter;
    };*/
    /*for(int type = 0; type < ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]; type++) {
        ImplicitGrapher::data->sequence[type] = ImplicitGrapher::sequences[ImplicitGrapher::surfaceEquation][type];
    }
    for(int type = 0; type < ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]; type++) {
        ImplicitGrapher::data->constants[type] = ImplicitGrapher::constants[ImplicitGrapher::surfaceEquation][type];
        ImplicitGrapher::data->equationValues[type] = ImplicitGrapher::equationValues[ImplicitGrapher::surfaceEquation][type];
    }
    ImplicitGrapher::data->valuesCounter = ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation];

    // Bind buffer object computeShaderVBO to indexed buffer target GL_SHADER_STORAGE_BUFFER at index indexBufferBinding
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ImplicitGrapher::indexBufferBinding, ImplicitGrapher::computeShaderVBO);
    // Create and initialize data store for buffer object computeShaderVBO with a size sizeof(GPUdata) and dynamically copy data to it from pointer location &data
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ImplicitGrapher::GPUdata), ImplicitGrapher::data, GL_DYNAMIC_COPY);// STREAM = infrequent use and changes, STATIC = frequent use and infrequent changes, DYNAMIC = frequent use and frequent changes
    glUseProgram(ImplicitGrapher::computeShaderProgram);
    glUniform3fv(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "currentOffset"), 1, (ImplicitGrapher::defaultOffset + ImplicitGrapher::offset).v);
    glUniform1f(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "t"), 0.1f * getFrameCount());
    glUniform1f(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "zoom"), 0.15f);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "valuesCounter"), ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "sequenceLength"), ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "surfaceEquation"), ImplicitGrapher::surfaceEquation);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "iterations"), ImplicitGrapher::iterations);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "vectorPointsPositive"), ImplicitGrapher::vectorPointsPositive);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "clipEdges"), ImplicitGrapher::clipEdges);
    // Launch work group
    glDispatchCompute(1, 1, 1);
    // Define the end of the ongoing GPU computation as the barrier after which the CPU code may continue to execute
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    // Bind buffer object computeShaderVBO to target GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, ImplicitGrapher::computeShaderVBO);
    // Map a section of buffer object computeShaderVBO's data store
    ImplicitGrapher::data = (ImplicitGrapher::GPUdata*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ImplicitGrapher::GPUdata), GL_MAP_READ_BIT);
    // Unmap buffer object computeShaderVBO's data store
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    {
        /*struct chunk {
        bool plusMinus[32];
        ivec3 xyzLineIndex[32];
        PositionXYZNormalXYZ vertices[3 * 32];
        uvec3 indices[3 * 3 * 32];
        int solutionCount;
        int numIndices;
        bool padding[2008];
        };*/
        for(int i = 0; i < 1024; i++) {
            for(int j = 0; j < 32; j++) {
                //ALOGD("ImplicitGrapher::data->chunks[%d].plusMinus[%d] = %s\n", type, j, to_string(ImplicitGrapher::data->chunks[type].plusMinus[j]).c_str());
                //ImplicitGrapher::data->chunks[type].xyzLineIndex[j] = ivec3(0);
                for(int k = 0; k < 3; k++) {
                    //ImplicitGrapher::data->chunks[type].vertices[3 * j + k] = PositionXYZNormalXYZ(vec3(0.0f), vec3(0.0f));
                    for(int l = 0; l < 3; l++) {
                        //ImplicitGrapher::data->chunks[type].indices[9 * j + 3 * k + l] = ivec3(0);
                    }
                }
            }
            //ImplicitGrapher::data->chunks[type].solutionCount = 0;
            //ImplicitGrapher::data->chunks[type].numIndices = 0;
            for(int j = 0; j < sizeof(ImplicitGrapher::chunk::padding); j++) {
                //ImplicitGrapher::data->chunks[type].padding[j] = false;
            }
        }
        /*struct GPUdata {
            chunk chunks[1024];
            ivec3 sequence[maxEquationLength];
            int constants[maxEquationLength];
            float equationValues[maxEquationLength];
            int valuesCounter;
        };*/
        for(int i = 0; i < ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]; i++) {
            //ImplicitGrapher::data->sequence[type] = ImplicitGrapher::sequences[ImplicitGrapher::surfaceEquation][type];
        }
        for(int i = 0; i < ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]; i++) {
            //ImplicitGrapher::data->constants[type] = ImplicitGrapher::constants[ImplicitGrapher::surfaceEquation][type];
            //ImplicitGrapher::data->equationValues[type] = ImplicitGrapher::equationValues[ImplicitGrapher::surfaceEquation][type];
        }
        //ImplicitGrapher::data->valuesCounter = ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation];
    }

    //ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    /*glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    for(int type = 0; type < 1024; type++) {
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[type].vertices[0].p);
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[type].vertices[0].n);
        glDrawElements(GL_TRIANGLES, ImplicitGrapher::data->chunks[type].numIndices, GL_UNSIGNED_INT, ImplicitGrapher::data->chunks[type].indices);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    for(int type = 0; type < 1024; type++){

    }*/

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);




















    simulation.simulate(false);
    glUseProgram(cubeProgram);
    for(int i = 0; i < numCacheChunks; i++){
        for(int j = 0; j < starsPerChunk && starsPerChunk * i + j < COUNT; j++){
            Matrix4<float> translation2;
            translation2.SetTranslation(Vec3<float>(
                    data->chunks[i].stars[j].position.x,
                    data->chunks[i].stars[j].position.y,
                    data->chunks[i].stars[j].position.z
            ));
            mvp = orientationAdjustedPerspective * translation * rotation * translation2;
            glUniformMatrix4fv(
                    glGetUniformLocation(cubeProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            cubeVAO.draw();
            //ALOGI("data->chunks[type].stars[j].position = %s\n", data->chunks[type].stars[j].position.str().c_str());
        }
    }
}
