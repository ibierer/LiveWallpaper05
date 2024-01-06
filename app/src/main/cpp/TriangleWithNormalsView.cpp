//
// Created by Immanuel Bierer on 12/5/2023.
//

#include "TriangleWithNormalsView.h"

TriangleWithNormalsView::TriangleWithNormalsView() : View(){
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    triangleWithNormalsVAO = VertexArrayObject(triangleVerticesNormals, sizeof(triangleVerticesNormals) / sizeof(PositionXYZNormalXYZ));
}

TriangleWithNormalsView::~TriangleWithNormalsView(){
    glDeleteProgram(mProgram);
}

void TriangleWithNormalsView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

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

    triangleWithNormalsVAO.draw();

    glDisable(GL_DEPTH_TEST);
}
