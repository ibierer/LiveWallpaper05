//
// Created by Immanuel Bierer on 12/24/2023.
//

#include "SphereView.h"

SphereView::SphereView() : View() {
    sphereProgram = createVertexAndFragmentShaderProgram(SPHERE_VERTEX_SHADER.c_str(), SPHERE_FRAGMENT_SHADER.c_str());
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
}

SphereView::~SphereView(){
    glDeleteProgram(sphereProgram);
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
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(sphereProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    sphereVAO.drawArrays();
}
