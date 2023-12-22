//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "RenderToCubeMapView.h"

RenderToCubeMapView::RenderToCubeMapView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mPlanesProgram = createProgram(PLANES_VERTEX_SHADER.c_str(), PLANES_FRAGMENT_SHADER.c_str());
    //texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    texture = Texture(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    cubeMapFBO = CubeMapFBO(
            CubeMap(GL_RGB, GL_LINEAR, 256, 0),
            YES,
            NO);
}

RenderToCubeMapView::~RenderToCubeMapView(){
    glDeleteProgram(mProgram);
}

template<class T>
Vec3<T> transform(Matrix3<T> matrix, const Vec3<T>& vector){
    /*float ax = matrix.GetRow(0)[0] * vector.x;
    float by = matrix.GetRow(0)[1] * vector.y;
    float cz = matrix.GetRow(0)[2] * vector.z;
    float dx = matrix.GetRow(1)[0] * vector.x;
    float ey = matrix.GetRow(1)[1] * vector.y;
    float fz = matrix.GetRow(1)[2] * vector.z;
    float gx = matrix.GetRow(2)[0] * vector.x;
    float hy = matrix.GetRow(2)[1] * vector.y;
    float iz = matrix.GetRow(2)[2] * vector.z;*/
    Vec3<T> result = Vec3<T>((T)0, (T)0, (T)0);
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            result[i] += matrix.GetRow(j)[i] * vector[j];
        }
        if(i > 0){
            result[i] *= (T)(-1);
        }
    }
    return result;
}

void RenderToCubeMapView::render(){
    int storeWidth = width;
    width = cubeMapFBO.getResolution();
    int storeHeight = height;
    height = cubeMapFBO.getResolution();
    calculatePerspective(90.0f);
    glViewport(0, 0, width, height);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    Matrix4<float> rotation;

    glUseProgram(mPlanesProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mPlanesProgram, "image"), 0);

    for(int i = 0; i < 6; i++){
        Vertex vertices[8] = {
                {vec3(0.0f, 0.0f, 0.0f)},
                {vec3(0.0f, 1.0f, 0.0f)},
                {vec3(1.0f, 0.0f, 0.0f)},
                {vec3(1.0f, 1.0f, 0.0f)},
                {vec3(0.0f, 0.0f, 1.0f)},
                {vec3(0.0f, 1.0f, 1.0f)},
                {vec3(1.0f, 0.0f, 1.0f)},
                {vec3(1.0f, 1.0f, 1.0f)}
        };
        uvec3 indices[4] = {
                uvec3(0, 2, 1),
                uvec3(1, 3, 2),
                uvec3(4, 6, 5),
                uvec3(5, 7, 6)
        };
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);

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

        Vec3<float> position = Vec3<float>(0.0f, 0.0f, 3.0f * (zoom - 1.0f));
        Matrix4<float> rotation2;
        rotation2.SetRotation(Vec3<float>(0.0f, 0.0f, 1.0f), 0.005 * getFrameCount());
        Matrix4<float> rotation3 = Matrix4<float>(quaternionTo3x3(rotationVector));
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
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);
    }

    width = storeWidth;
    height = storeHeight;
    calculatePerspective(60.0f);
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

    glDisable(GL_DEPTH_TEST);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}
