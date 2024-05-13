//
// Created by Immanuel Bierer on 12/29/2023.
//

#include "SphereWithReflectionView.h"

SphereWithReflectionView::SphereWithReflectionView() : View() {
    sphereMapReflectionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                      SPHERE_MAP_REFLECTION_FRAGMENT_SHADER.c_str());
    sphereMapBackgroundProgram = createVertexAndFragmentShaderProgram(
            BACKGROUND_VERTEX_SHADER.c_str(),
            SPHERE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    cubeMapReflectionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                    CUBE_MAP_REFLECTION_FRAGMENT_SHADER.c_str());
    cubeMapBackgroundProgram = createVertexAndFragmentShaderProgram(
            BACKGROUND_VERTEX_SHADER.c_str(),
            CUBE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    environmentMap = CubeMap::createSimpleTextureCubemap();
    //environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));
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
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -20.0f * distanceToOrigin));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();


    glUseProgram(cubeMapReflectionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapReflectionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapReflectionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.drawArrays();


    glUseProgram(cubeMapBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.drawArrays();


    /*glUseProgram(sphereMapReflectionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapReflectionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapReflectionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.draw();


    glUseProgram(sphereMapBackgroundProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapBackgroundProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    environmentTriangleVAO.drawArrays();*/
}
