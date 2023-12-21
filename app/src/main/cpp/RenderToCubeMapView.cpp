//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "RenderToCubeMapView.h"

RenderToCubeMapView::RenderToCubeMapView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mPlanesProgram = createProgram(PLANES_VERTEX_SHADER.c_str(), PLANES_FRAGMENT_SHADER.c_str());
    texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    //texture = Texture(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    cubeMapFBO = CubeMapFBO(
            CubeMap(GL_RGB, GL_LINEAR, 256, 0),
            YES,
            NO);
}

RenderToCubeMapView::~RenderToCubeMapView(){
    glDeleteProgram(mProgram);
}

void RenderToCubeMapView::render(){
    int storeWidth = width;
    width = cubeMapFBO.getResolution();
    int storeHeight = height;
    height = cubeMapFBO.getResolution();
    calculatePerspective(90.0f);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    Matrix4<float> rotation;

    glUseProgram(mPlanesProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mPlanesProgram, "image"), 0);

    for(int i = 0; i < 6; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO.frameBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, cubeMapFBO.drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapFBO.cubeMap.getTextureId(), 0);
        Vertex vertices[8] = {
                {vec3(0.0f, 0.0f, -0.25f)},
                {vec3(0.0f, 1.0f, -0.25f)},
                {vec3(1.0f, 0.0f, -0.25f)},
                {vec3(1.0f, 1.0f, -0.25f)},
                {vec3(0.0f, 0.0f, 0.25f)},
                {vec3(0.0f, 1.0f, 0.25f)},
                {vec3(1.0f, 0.0f, 0.25f)},
                {vec3(1.0f, 1.0f, 0.25f)}
        };
        uvec3 indices[4] = {
                uvec3(0, 2, 1),
                uvec3(1, 3, 2),
                uvec3(4, 6, 5),
                uvec3(5, 7, 6)
        };
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);

        /*switch(i){
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
        }*/

        switch(i){
            case 0:
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), M_PI / 2.0);
                glClearColor(1.0f, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                break;
            case 1:
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), -M_PI / 2.0);
                glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                break;
            case 2:
                rotation.SetRotation(Vec3<float>(1.0f, 0.0f, 0.0f), M_PI / 2.0f);
                glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                break;
            case 3:
                rotation.SetRotation(Vec3<float>(1.0f, 0.0f, 0.0f), -M_PI / 2.0);
                glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                break;
            case 4:
                rotation.SetIdentity();
                glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                break;
            case 5:
                glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
                rotation.SetRotation(Vec3<float>(0.0f, 1.0f, 0.0f), M_PI);
                break;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Vec3<float> position = Vec3<float>(0.0f, 0.0f, 3.0f * (zoom - 1.0f));
        Matrix4<float> rotation2;
        rotation2.SetRotation(Vec3<float>(0.0f, 0.0f, 1.0f), 0.005 * getFrameCount());
        Matrix4<float> rotation3 = Matrix4<float>(quaternionTo3x3(rotationVector));
        Matrix3<float> subMatrix = rotation3.GetSubMatrix3();
        /*float ax = subMatrix.GetRow(0)[0] * position.x;
        float by = subMatrix.GetRow(0)[1] * position.y;
        float cz = subMatrix.GetRow(0)[2] * position.z;
        float dx = subMatrix.GetRow(1)[0] * position.x;
        float ey = subMatrix.GetRow(1)[1] * position.y;
        float fz = subMatrix.GetRow(1)[2] * position.z;
        float gx = subMatrix.GetRow(2)[0] * position.x;
        float hy = subMatrix.GetRow(2)[1] * position.y;
        float iz = subMatrix.GetRow(2)[2] * position.z;*/
        float ax = subMatrix.GetRow(0)[0] * position.x;
        float by = subMatrix.GetRow(1)[0] * position.y;
        float cz = subMatrix.GetRow(2)[0] * position.z;
        float dx = subMatrix.GetRow(0)[1] * position.x;
        float ey = subMatrix.GetRow(1)[1] * position.y;
        float fz = subMatrix.GetRow(2)[1] * position.z;
        float gx = subMatrix.GetRow(0)[2] * position.x;
        float hy = subMatrix.GetRow(1)[2] * position.y;
        float iz = subMatrix.GetRow(2)[2] * position.z;
        Vec3<float> transpose = Vec3<float>(ax + by + cz, -(dx + ey + fz), -(gx + hy + iz));
        Matrix4<float> translation;
        translation = translation.Translation(transpose);
        Matrix4<float> translation2;
        translation2 = translation2.Translation(Vec3<float>(-0.5f, -0.5f, 0.0f));
        //Matrix4<float> mvp = orientationAdjustedPerspective * rotation * rotation2 * translation2;
        Matrix4<float> mvp = orientationAdjustedPerspective * rotation * translation * rotation2 * translation2;

        glUniformMatrix4fv(
                glGetUniformLocation(mPlanesProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat*)&mvp);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);
    }

    width = storeWidth;
    height = storeHeight;
    calculatePerspective(45.0f);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //calculatePerspective(60.0f * (1.0f - zoom));
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
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

    Vertex vertices[3] = {
            {vec3(-1.0f, -1.0f, 0.999f)},
            {vec3( 3.0f, -1.0f, 0.999f)},
            {vec3(-1.0f,  3.0f, 0.999f)}
    };
    uvec3 indices[1] = {
            uvec3(0, 1, 2)
    };
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}
