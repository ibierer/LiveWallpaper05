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
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    //ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);
    glUseProgram(ImplicitGrapher::computeShaderProgram);
    glUniform3fv(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "currentOffset"), 1, (ImplicitGrapher::defaultOffset + ImplicitGrapher::offset).v);
    glUniform1f(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "t"), 0.1f * getFrameCount());
    glUniform1f(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "zoom"), 0.15f);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "valuesCounter"), ImplicitGrapher::valuesCounter[ImplicitGrapher::surfaceEquation]);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "sequenceLength"), ImplicitGrapher::sequenceLengths[ImplicitGrapher::surfaceEquation]);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "surfaceEquation"), ImplicitGrapher::surfaceEquation);
    glUniform1i(glGetUniformLocation(ImplicitGrapher::computeShaderProgram, "iterations"), ImplicitGrapher::iterations);
    ImplicitGrapher::calculateSurfaceOnGPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);

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
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
}
