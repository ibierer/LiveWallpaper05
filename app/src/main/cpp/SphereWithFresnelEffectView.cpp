//
// Created by Immanuel Bierer on 12/31/2023.
//

#include "SphereWithFresnelEffectView.h"

SphereWithFresnelEffectView::SphereWithFresnelEffectView(Texture::DefaultImages option, const int &resolution) : View() {
    sphereMapRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                      SPHERE_MAP_REFRACTION_FRAGMENT_SHADER.c_str());
    sphereMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                            SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
    sphereMapBackgroundProgram = createVertexAndFragmentShaderProgram(
            BACKGROUND_VERTEX_SHADER.c_str(),
            SPHERE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    cubeMapRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                    CUBE_MAP_REFRACTION_FRAGMENT_SHADER.c_str());
    cubeMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),
                                                                          CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
    cubeMapBackgroundProgram = createVertexAndFragmentShaderProgram(
            BACKGROUND_VERTEX_SHADER.c_str(),
            CUBE_MAP_BACKGROUND_FRAGMENT_SHADER.c_str());
    //environmentMap = CubeMap::createSimpleTextureCubemap();
    //environmentMap = SphereMap(option, resolution, resolution, this);
    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        environmentMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
    }
    sphereVAO = VertexArrayObject(Sphere(1.0f, 100));
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

SphereWithFresnelEffectView::~SphereWithFresnelEffectView(){
    glDeleteProgram(mProgram);
}

void SphereWithFresnelEffectView::render() {
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -20.0f * distanceToOrigin));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(
            Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix3<float> inverse3x3Transpose;
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    inverse3x3Transpose = rotation.GetSubMatrix3().Identity();
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glBindTexture(GL_TEXTURE_2D, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    /*glUseProgram(cubeMapRefractionProgram);
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

    if(!backgroundIsSolidColor) {
        glUseProgram(cubeMapBackgroundProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(cubeMapBackgroundProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat*)&inverseViewProjection);

        environmentTriangleVAO.drawArrays();
    }*/


    /*glUseProgram(sphereMapRefractionProgram);
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


    if(!backgroundIsSolidColor) {
        glUseProgram(sphereMapBackgroundProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(sphereMapBackgroundProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat*)&inverseViewProjection);

        environmentTriangleVAO.drawArrays();
    }*/


    glUseProgram(sphereMapDoubleRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapDoubleRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat *) &mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapDoubleRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat *) &cameraTransformation);

    sphereVAO.drawArrays();


    if (!backgroundIsSolidColor) {
        glUseProgram(sphereMapBackgroundProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(sphereMapBackgroundProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat *) &inverseViewProjection);

        environmentTriangleVAO.drawArrays();
    }


    /*glUseProgram(cubeMapDoubleRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapDoubleRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeMapDoubleRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat*)&cameraTransformation);

    sphereVAO.drawArrays();


    if(!backgroundIsSolidColor) {
        glUseProgram(cubeMapBackgroundProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(cubeMapBackgroundProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat*)&inverseViewProjection);

        environmentTriangleVAO.drawArrays();
    }*/
}
