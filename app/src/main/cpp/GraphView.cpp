//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;



















#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <GLES3/gl31.h>

class ComputeShader {
public:
    GLuint gComputeProgram;
    GLuint gVBO;
    static const GLuint gIndexBufferBinding = 0;
    std::string filepath = "/sdcard/Android/data/com.outputlog/files/log.txt";
    GLuint generateComputeShader(const char* computeShaderSrcCode) {
        // Create the compute program, to which the compute shader will be assigned
        GLuint gComputeProgram = glCreateProgram();
        // Create and compile the compute shader
        GLuint mComputeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(mComputeShader, 1, &computeShaderSrcCode, NULL);
        glCompileShader(mComputeShader);
        // Check if there were any issues when compiling the shader
        int rvalue;
        glGetShaderiv(mComputeShader, GL_COMPILE_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetShaderiv(mComputeShader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetShaderInfoLog(mComputeShader, infoLen, NULL, log);
                //esLogMessage("Error compiling shader:\n%s\n", log);
                //outputLog(log, infoLen);
                //myLogger.outputLog(std::string("Error compiling shader:\n") + std::string(log), filepath);
                free(log);
            }
            glDeleteShader(mComputeShader);
            return 0;
            //glGetShaderInfoLog(mComputeShader, LOG_MAX, &length, log);
            //printf("Error: Compiler log:\n%s\n", log);
            //return false;
        }
        // Attach and link the shader against to the compute program
        glAttachShader(gComputeProgram, mComputeShader);
        glLinkProgram(gComputeProgram);
        // Check if there were some issues when linking the shader.
        glGetProgramiv(gComputeProgram, GL_LINK_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetProgramiv(gComputeProgram, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetProgramInfoLog(gComputeProgram, infoLen, NULL, log);
                //esLogMessage("Error linking program:\n%s\n", log);
                //outputLog(log, infoLen);
                //myLogger.outputLog(std::string("Error linking program:\n") + std::string(log), filepath);
                free(log);
            }
            glDeleteProgram(gComputeProgram);
            return 0;
            //glGetProgramInfoLog(gComputeProgram, LOG_MAX, &length, log);
            //printf("Error: Linker log:\n%s\n", log);
            //return false;
        }
        return gComputeProgram;
    }
    static GLuint compileComputeShader(const char* computeShaderSrcCode){
        // Create the compute program, to which the compute shader will be assigned
        GLuint gComputeProgram = glCreateProgram();
        // Create and compile the compute shader
        GLuint mComputeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(mComputeShader, 1, &computeShaderSrcCode, NULL);
        glCompileShader(mComputeShader);
        // Check if there were any issues when compiling the shader
        int rvalue;
        glGetShaderiv(mComputeShader, GL_COMPILE_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetShaderiv(mComputeShader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetShaderInfoLog(mComputeShader, infoLen, NULL, log);
                //esLogMessage("Error compiling shader:\n%s\n", log);
                //outputLog(log, infoLen);
                //myLogger::outputLog(std::string("Error compiling shader:\n") + std::string(log), filepath);
                free(log);
            }
            glDeleteShader(mComputeShader);
            return 0;
            //glGetShaderInfoLog(mComputeShader, LOG_MAX, &length, log);
            //printf("Error: Compiler log:\n%s\n", log);
            //return false;
        }
        // Attach and link the shader against to the compute program
        glAttachShader(gComputeProgram, mComputeShader);
        glLinkProgram(gComputeProgram);
        // Check if there were some issues when linking the shader.
        glGetProgramiv(gComputeProgram, GL_LINK_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetProgramiv(gComputeProgram, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetProgramInfoLog(gComputeProgram, infoLen, NULL, log);
                //esLogMessage("Error linking program:\n%s\n", log);
                //outputLog(log, infoLen);
                //myLogger::outputLog(std::string("Error linking program:\n") + std::string(log), filepath);
                free(log);
            }
            glDeleteProgram(gComputeProgram);
            return 0;
            //glGetProgramInfoLog(gComputeProgram, LOG_MAX, &length, log);
            //printf("Error: Linker log:\n%s\n", log);
            //return false;
        }
        return gComputeProgram;
    }
};

#endif
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

VertexArrayObject cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

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

GLuint cubeProgram = View::createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
















GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    implicitGrapher = ImplicitGrapher(ivec3(29));
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;
    /*glGenBuffers(1, &ImplicitGrapher::computeShaderVBO);
    ImplicitGrapher::indexBufferBinding = 0;*/




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
    /*for(int i = 0; i < 1024; i++) {
        for(int j = 0; j < 32; j++) {
            ImplicitGrapher::data->chunks[i].plusMinus[j] = false;
            ImplicitGrapher::data->chunks[i].xyzLineIndex[j] = ivec3(0);
            for(int k = 0; k < 3; k++) {
                ImplicitGrapher::data->chunks[i].vertices[3 * j + k] = PositionXYZNormalXYZ(vec3(0.0f), vec3(0.0f));
                for(int l = 0; l < 3; l++) {
                    ImplicitGrapher::data->chunks[i].indices[9 * j + 3 * k + l] = ivec3(0);
                }
            }
        }
        ImplicitGrapher::data->chunks[i].solutionCount = 0;
        ImplicitGrapher::data->chunks[i].numIndices = 0;
        for(int j = 0; j < sizeof(ImplicitGrapher::chunk::padding); j++) {
            ImplicitGrapher::data->chunks[i].padding[j] = false;
        }
    }*/
    /*struct GPUdata {
        chunk chunks[1024];
        ivec3 sequence[maxEquationLength];
        int constants[maxEquationLength];
        float equationValues[maxEquationLength];
        int valuesCounter;
    };*/
    /*for(int i = 0; i < ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]; i++) {
        ImplicitGrapher::data->sequence[i] = ImplicitGrapher::sequences[ImplicitGrapher::surfaceEquation][i];
    }
    for(int i = 0; i < ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]; i++) {
        ImplicitGrapher::data->constants[i] = ImplicitGrapher::constants[ImplicitGrapher::surfaceEquation][i];
        ImplicitGrapher::data->equationValues[i] = ImplicitGrapher::equationValues[ImplicitGrapher::surfaceEquation][i];
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
                //ALOGD("ImplicitGrapher::data->chunks[%d].plusMinus[%d] = %s\n", i, j, to_string(ImplicitGrapher::data->chunks[i].plusMinus[j]).c_str());
                //ImplicitGrapher::data->chunks[i].xyzLineIndex[j] = ivec3(0);
                for(int k = 0; k < 3; k++) {
                    //ImplicitGrapher::data->chunks[i].vertices[3 * j + k] = PositionXYZNormalXYZ(vec3(0.0f), vec3(0.0f));
                    for(int l = 0; l < 3; l++) {
                        //ImplicitGrapher::data->chunks[i].indices[9 * j + 3 * k + l] = ivec3(0);
                    }
                }
            }
            //ImplicitGrapher::data->chunks[i].solutionCount = 0;
            //ImplicitGrapher::data->chunks[i].numIndices = 0;
            for(int j = 0; j < sizeof(ImplicitGrapher::chunk::padding); j++) {
                //ImplicitGrapher::data->chunks[i].padding[j] = false;
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
            //ImplicitGrapher::data->sequence[i] = ImplicitGrapher::sequences[ImplicitGrapher::surfaceEquation][i];
        }
        for(int i = 0; i < ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]; i++) {
            //ImplicitGrapher::data->constants[i] = ImplicitGrapher::constants[ImplicitGrapher::surfaceEquation][i];
            //ImplicitGrapher::data->equationValues[i] = ImplicitGrapher::equationValues[ImplicitGrapher::surfaceEquation][i];
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
    for(int i = 0; i < 1024; i++) {
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[i].vertices[0].p);
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[i].vertices[0].n);
        glDrawElements(GL_TRIANGLES, ImplicitGrapher::data->chunks[i].numIndices, GL_UNSIGNED_INT, ImplicitGrapher::data->chunks[i].indices);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    for(int i = 0; i < 1024; i++){

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
            translation.SetTranslation(Vec3<float>(
                    data->chunks[i].stars[j].position.x,
                    data->chunks[i].stars[j].position.y,
                    data->chunks[i].stars[j].position.z
            ));
translation.SetTranslation(Vec3<float>(0.0f));
            mvp = orientationAdjustedPerspective * translation * rotation;
            glUniformMatrix4fv(
                    glGetUniformLocation(cubeProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            cubeVAO.draw();
            ALOGI("data->chunks[i].stars[j].position = %s\n", data->chunks[i].stars[j].position.str().c_str());
        }
    }
}
