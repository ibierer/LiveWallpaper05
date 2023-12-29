//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMapView.h"

SphereMapView::SphereMapView() : View() {
    //sphereMap = SphereMap(Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this));
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    //sphereMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    sphereMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    environmentTriangleVAO = VertexArrayObject(sphereMap.environmentTriangleVertices, sizeof(sphereMap.environmentTriangleVertices) / sizeof(Vertex));
}

SphereMapView::~SphereMapView(){
    glDeleteProgram(mProgram);
}

void SphereMapView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    calculatePerspectiveSetViewport(60.0f * (1.0f - zoom));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();
}