//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "RenderToCubeMapView.h"

RenderToCubeMapView::RenderToCubeMapView() : View() {
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mPlanesProgram = createVertexAndFragmentShaderProgram(PLANES_VERTEX_SHADER.c_str(),
                                                          PLANES_FRAGMENT_SHADER.c_str());
    tilesVAO = VertexArrayObject(tilesVertices, sizeof(tilesVertices) / sizeof(PositionXYZ));
    //texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    texture = Texture(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    cubeMapFBO = CubeMapFBO(
            CubeMap(GL_RGB, GL_LINEAR, 256, 0),
            YES,
            NO);
    environmentTriangleVAO = VertexArrayObject(CubeMap::environmentTriangleVertices, sizeof(cubeMapFBO.cubeMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

RenderToCubeMapView::~RenderToCubeMapView(){
    glDeleteProgram(mProgram);
}

void RenderToCubeMapView::render(){
    int storeWidth = width;
    width = cubeMapFBO.getResolution();
    int storeHeight = height;
    height = cubeMapFBO.getResolution();
    calculatePerspectiveSetViewport(90.0f, zNear, zFar);

    Matrix4<float> rotation;

    glUseProgram(mPlanesProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mPlanesProgram, "image"), 0);

    for(int i = 0; i < 6; i++){
        switch(i){
            case 0:
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), M_PI / 2.0);
                break;
            case 1:
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), -M_PI / 2.0);
                break;
            case 2:
                rotation.SetRotation(Vec3<float>(1.0f, 0.0f, 0.0f), M_PI / 2.0f);
                break;
            case 3:
                rotation.SetRotation(Vec3<float>(1.0f, 0.0f, 0.0f), -M_PI / 2.0);
                break;
            case 4:
                rotation.SetIdentity();
                break;
            case 5:
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), M_PI);
                break;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO.frameBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, cubeMapFBO.drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapFBO.cubeMap.getTextureId(), 0);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Vec3<float> position = Vec3<float>(0.0f, 0.0f, -6.0f * distanceToOrigin);
        Matrix4<float> rotation2;
        rotation2.SetRotation(Vec3<float>(0.0f, 0.0f, 1.0f), 0.005 * getFrameCount());
        Matrix4<float> rotation3 = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
        Matrix3<float> subMatrix = rotation3.GetSubMatrix3();
        Vec3<float> transpose = transform(subMatrix, position);
        Matrix4<float> translation;
        translation = translation.Translation(transpose);
        Matrix4<float> translation2;
        translation2 = translation2.Translation(Vec3<float>(-0.5f));
        //Matrix4<float> mvp = orientationAdjustedPerspective * rotation * rotation2 * translation2;
        Matrix4<float> mvp = orientationAdjustedPerspective * rotation * translation * rotation2 * translation2;

        glUniformMatrix4fv(
                glGetUniformLocation(mPlanesProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat*)&mvp);
        tilesVAO.drawArrays();
    }

    width = storeWidth;
    height = storeHeight;
    calculatePerspectiveSetViewport(maxViewAngle, zNear, zFar);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapFBO.cubeMap.getTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "environmentTexture"), 1);

    environmentTriangleVAO.drawArrays();
}
