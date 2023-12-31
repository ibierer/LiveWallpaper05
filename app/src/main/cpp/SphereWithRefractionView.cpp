//
// Created by Immanuel Bierer on 12/29/2023.
//

#include "SphereWithRefractionView.h"

SphereWithRefractionView::SphereWithRefractionView() : View() {
    sphereMapRefractionProgram = createProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_REFRACTION_FRAGMENT_SHADER.c_str());
    sphereMapDoubleRefractionProgram = createProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
    sphereMapBackgroundProgram = createProgram(BACKGROUND_VERTEX_SHADER.c_str(), SPHERE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    cubeMapRefractionProgram = createProgram(VERTEX_SHADER.c_str(), CUBE_MAP_REFRACTION_FRAGMENT_SHADER.c_str());
    cubeMapDoubleRefractionProgram = createProgram(VERTEX_SHADER.c_str(), CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
    cubeMapBackgroundProgram = createProgram(BACKGROUND_VERTEX_SHADER.c_str(), CUBE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    //environmentMap = CubeMap::createSimpleTextureCubemap();
    environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
    environmentTriangleVAO = VertexArrayObject(environmentMap.environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

SphereWithRefractionView::~SphereWithRefractionView(){
    glDeleteProgram(mProgram);
}

void SphereWithRefractionView::render(){
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

    glUseProgram(cubeMapRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(cubeMapBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();*/


    /*glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;

    glUseProgram(sphereMapRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(sphereMapBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, environmentMap.getTextureId());
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
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;

    glUseProgram(sphereMapDoubleRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapDoubleRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapDoubleRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(sphereMapBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.draw();
}
