//
// Created by Immanuel Bierer on 12/24/2023.
//

#include "SphereView.h"

SphereView::SphereView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    sphere = Sphere(1.0f, 100);
    sphereVAO = VertexArrayObject(sphere);
}

SphereView::~SphereView(){
    glDeleteProgram(mProgram);
}

void SphereView::render(){
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

    glBindVertexArray(sphereVAO.getArrayObjectId());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere.getNumVertices());
    glBindVertexArray(0);
}
