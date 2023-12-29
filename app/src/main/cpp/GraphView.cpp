//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

GraphView::GraphView() : View(), implicitGrapher(ImplicitGrapher(20)){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    implicitGrapher.surfaceEquation = 1;
}

GraphView::GraphView(const string& equation) : View(), implicitGrapher(ImplicitGrapher(20)){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    implicitGrapher.surfaceEquation = implicitGrapher.numOfEquationsInMemory;
    implicitGrapher.memoryEquations[implicitGrapher.numOfEquationsInMemory][1] = equation;
    implicitGrapher.processEquation(implicitGrapher.numOfEquationsInMemory);
    implicitGrapher.numOfEquationsInMemory++;
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

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

    implicitGrapher.calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &implicitGrapher.vertices[0], implicitGrapher.indices, implicitGrapher.numIndices);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&implicitGrapher.vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&implicitGrapher.vertices[0].n);
    glDrawElements(GL_TRIANGLES, implicitGrapher.numIndices, GL_UNSIGNED_INT, implicitGrapher.indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
}
