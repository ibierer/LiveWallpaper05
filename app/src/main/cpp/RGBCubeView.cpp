//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "RGBCubeView.h"

RGBCubeView::RGBCubeView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    float radius = 0.5f;
    mBoxVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::RGB_CUBE));
}

RGBCubeView::~RGBCubeView(){
    glDeleteProgram(mProgram);
}

void RGBCubeView::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, zoom - 2.0f));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    mBoxVAO.draw();

    checkGlError("Renderer::render");
}
