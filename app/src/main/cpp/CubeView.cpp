//
// Created by Immanuel Bierer on 12/28/2023.
//

#include "CubeView.h"

CubeView::CubeView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cube = Cube(1.0f, 100);
    cubeVAO = VertexArrayObject(cube.getVertices(), cube.getNumVertices() * sizeof(VertexNormal));
}

CubeView::~CubeView(){
    glDeleteProgram(mProgram);
}

void CubeView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    glBindVertexArray(cubeVAO.getArrayObjectId());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, cube.getNumVertices());
    glBindVertexArray(0);
}