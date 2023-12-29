//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "TriangleView.h"

TriangleView::TriangleView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    Vertex vertices[3] = {
            {vec3(1.0f, 0.0f, 0.0f)},
            {vec3(0.0f, 1.0f, 0.0f)},
            {vec3(1.0f, 0.0f, 1.0f)}
    };
    triangleVAO = VertexArrayObject(vertices, sizeof(vertices) / sizeof(Vertex));
}

TriangleView::~TriangleView(){
    glDeleteProgram(mProgram);
}

void TriangleView::render(){
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

    triangleVAO.draw();

    glDisable(GL_DEPTH_TEST);
}
