//
// Created by Immanuel Bierer on 12/11/2023.
//

#include "CubeMapView.h"

CubeMapView::CubeMapView() : View(){
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cubeMap = CubeMap::createSimpleTextureCubemap();
    environmentTriangleVAO = VertexArrayObject(CubeMap::environmentTriangleVertices, sizeof(cubeMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

CubeMapView::~CubeMapView(){
    glDeleteProgram(mProgram);
}

void CubeMapView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.drawArrays();
}
