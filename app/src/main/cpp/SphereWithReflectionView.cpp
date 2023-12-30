//
// Created by Immanuel Bierer on 12/29/2023.
//

#include "SphereWithReflectionView.h"

SphereWithReflectionView::SphereWithReflectionView() : View() {
    mProgram = createProgram(_BACKGROUND_VERTEX_SHADER.c_str(), _BACKGROUND_FRAGMENT_SHADER.c_str());
    _mProgram = createProgram(_REFLECTION_VERTEX_SHADER.c_str(), _REFLECTION_FRAGMENT_SHADER.c_str());
    mReflectionProgram = createProgram(REFLECTION_VERTEX_SHADER.c_str(), REFLECTION_FRAGMENT_SHADER.c_str());
    mBackgroundProgram = createProgram(BACKGROUND_VERTEX_SHADER.c_str(), BACKGROUND_FRAGMENT_SHADER.c_str());
    //environmentMap = CubeMap::createSimpleTextureCubemap();
    environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
    environmentTriangleVAO = VertexArrayObject(environmentMap.environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

SphereWithReflectionView::~SphereWithReflectionView(){
    glDeleteProgram(mProgram);
}

void SphereWithReflectionView::render(){
    /*glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix3<float> inverse3x3Transpose;
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    inverse3x3Transpose = rotation.GetSubMatrix3().Identity();
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;

    glUseProgram(mReflectionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mReflectionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(mReflectionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();*/


    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix3<float> inverse3x3Transpose;
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    inverse3x3Transpose = rotation.GetSubMatrix3().Identity();
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;

    glUseProgram(_mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(_mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(_mProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();
}
