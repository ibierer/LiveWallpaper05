//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    implicitGrapher = ImplicitGrapher(ivec3(29));
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;
    glGenBuffers(1, &ImplicitGrapher::computeShaderVBO);
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
    for(int i = 0; i < 1024; i++) {
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
    }
    /*struct GPUdata {
        chunk chunks[1024];
        ivec3 sequence[maxEquationLength];
        int constants[maxEquationLength];
        float equationValues[maxEquationLength];
        int valuesCounter;
    };*/
    for(int i = 0; i < ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]; i++) {
        ImplicitGrapher::data->sequence[i] = ImplicitGrapher::sequences[ImplicitGrapher::surfaceEquation][i];
    }
    for(int i = 0; i < ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]; i++) {
        ImplicitGrapher::data->constants[i] = ImplicitGrapher::constants[ImplicitGrapher::surfaceEquation][i];
        ImplicitGrapher::data->equationValues[i] = ImplicitGrapher::equationValues[ImplicitGrapher::surfaceEquation][i];
    }
    ImplicitGrapher::data->valuesCounter = 0;
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    for(int i = 0; i < 1024; i++) {
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[i].vertices[0].p);
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::data->chunks[i].vertices[0].n);
        glDrawElements(GL_TRIANGLES, ImplicitGrapher::data->chunks[i].numIndices, GL_UNSIGNED_INT, ImplicitGrapher::data->chunks[i].indices);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    for(int i = 0; i < 1024; i++){

    }

    /*glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);*/
}
