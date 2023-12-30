//
// Created by Immanuel Bierer on 12/29/2023.
//

#include "SphereWithReflectionView.h"

SphereWithReflectionView::SphereWithReflectionView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mReflectionProgram = createProgram(REFLECTION_VERTEX_SHADER.c_str(), REFLECTION_FRAGMENT_SHADER.c_str());
    mBackgroundProgram = createProgram(BACKGROUND_VERTEX_SHADER.c_str(), BACKGROUND_FRAGMENT_SHADER.c_str());
    cubeMap = CubeMap::createSimpleTextureCubemap();
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
    environmentTriangleVAO = VertexArrayObject(cubeMap.environmentTriangleVertices, sizeof(cubeMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

SphereWithReflectionView::~SphereWithReflectionView(){
    glDeleteProgram(mProgram);
}

void SphereWithReflectionView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();
    Matrix3<float> inverse3x3Transpose;
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;
    inverse3x3Transpose = mvp.GetSubMatrix3().GetInverse().GetTranspose();
    Matrix4<float> cameraTransformation = rotation.GetInverse() * (-translation);
    //inverse3x3Transpose = mvp.GetSubMatrix3();

    /*glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);*/
    glUseProgram(mReflectionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mReflectionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(mReflectionProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glUniformMatrix3fv(
            glGetUniformLocation(mReflectionProgram, "inverse3x3Transpose"),
            1,
            GL_FALSE,
            (GLfloat*)&inverse3x3Transpose);
    glUniformMatrix4fv(
            glGetUniformLocation(mReflectionProgram, "cameraTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    glUseProgram(mBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();
}
