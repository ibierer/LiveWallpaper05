//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "ShaderToyView.h"

ShaderToyView::ShaderToyView() : View() {
    //sphereMap = SphereMap(Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this));
    sphereMapProgram = createVertexAndFragmentShaderProgram(SPHERE_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
    //sphereMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    sphereMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(sphereMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

ShaderToyView::~ShaderToyView(){
    glDeleteProgram(mProgram);
}

void ShaderToyView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    calculatePerspectiveSetViewport(maxViewAngle, zNear, zFar);
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(sphereMapProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glUniform2fv(glGetUniformLocation(sphereMapProgram,"iResolution"), 1, vec2(width, height).v);
    glUniform1f(glGetUniformLocation(sphereMapProgram,"iTime"),(float)getFrameCount());
    glUniform2fv(glGetUniformLocation(sphereMapProgram,"iMouse"), 1, vec2(0.0f, 0.0f).v);
    glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.drawArrays();
}