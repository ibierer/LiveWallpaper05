//
// Created by Immanuel Bierer on 12/31/2023.
//

#include "SphereWithFresnelEffectView.h"

SphereWithFresnelEffectView::SphereWithFresnelEffectView() : View() {
    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        environmentMapBackgroundProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
        environmentMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        environmentMapBackgroundProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
        environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
    }else if(backgroundTexture == Texture::DefaultImages::RGB_CUBE){
        environmentMapTextureTarget = GL_TEXTURE_CUBE_MAP;
        environmentMap = CubeMap::createSimpleTextureCubemap();
        environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        environmentMapBackgroundProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), CUBE_MAP_FRAGMENT_SHADER.c_str());
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
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> view = translation * rotation;
    Matrix4<float> mvp = orientationAdjustedPerspective * view;
    Matrix4<float> cameraTransformation = rotation.GetInverse() * view;
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    glUseProgram(environmentMapDoubleRefractionProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(environmentMapDoubleRefractionProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat *) &mvp);
    glUniformMatrix4fv(
            glGetUniformLocation(environmentMapDoubleRefractionProgram, "viewTransformation"),
            1,
            GL_FALSE,
            (GLfloat *) &cameraTransformation);
    sphereVAO.drawArrays();

    if (!backgroundIsSolidColor) {
        // Render environment map
        glUseProgram(environmentMapBackgroundProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(environmentMapBackgroundProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat *) &inverseViewProjection);
        environmentTriangleVAO.drawArrays();
    }

    glBindTexture(environmentMapTextureTarget, 0);
}
