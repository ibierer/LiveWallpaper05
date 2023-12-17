//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "RenderToCubeMapView.h"

const int squareSize = 2048;

GLuint genCubeMap(GLint internalFormat, const GLsizei imageWidth, const GLsizei imageHeight, GLint param, GLubyte* cubemapPixelBuffers[6]) {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, cubemapPixelBuffers == NULL ? 0 : cubemapPixelBuffers[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureId;
}

RenderToCubeMapView::RenderToCubeMapView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    _mProgram = createProgram(_VERTEX_SHADER.c_str(), _FRAGMENT_SHADER.c_str());
    cubeMap = CubeMap::createSimpleTextureCubemap();
    texture = Texture(Texture::MS_PAINT_COLORS);
    cubeMap = CubeMap(genCubeMap(GL_RGB, squareSize, squareSize, GL_LINEAR, NULL));

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
    glGenFramebuffers(6, &frameBuffers[0]);
    glGenRenderbuffers(6, &depthAndStencilRenderBuffers[0]);
    for (int i = 0; i < 6; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndStencilRenderBuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, squareSize, squareSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndStencilRenderBuffers[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeMap.getTextureId(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //return frameBufferStatus != GL_FRAMEBUFFER_COMPLETE;
}

RenderToCubeMapView::~RenderToCubeMapView(){
    glDeleteProgram(mProgram);
}

void RenderToCubeMapView::render(){

    int storeWidth = width;
    width = squareSize;
    int storeHeight = height;
    height = squareSize;
    calculatePerspective(90.0f);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    Matrix4<float> rotation;

    glUseProgram(_mProgram);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
    glActiveTexture(GL_TEXTURE1);

    for(int i = 0; i < 6; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap.getTextureId(), 0);
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

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
        glUniform1i(glGetUniformLocation(_mProgram, "environmentTexture"), 0);

        Vertex vertices[8] = {
                {vec3(0.0f, 0.0f, -5.0f)},
                {vec3(0.0f, 1.0f, -5.0f)},
                {vec3(1.0f, 0.0f, -5.0f)},
                {vec3(1.0f, 1.0f, -5.0f)},
                {vec3(0.0f, 0.0f, 50.0f)},
                {vec3(0.0f, 1.0f, 50.0f)},
                {vec3(1.0f, 0.0f, 50.0f)},
                {vec3(1.0f, 1.0f, 50.0f)}
        };
        uvec3 indices[4] = {
                uvec3(0, 2, 1),
                uvec3(1, 3, 2),
                uvec3(4, 6, 5),
                uvec3(5, 7, 6)
        };
        for(int j = 0; j < 10; j++){
            for(int k = 0; k < 10; k++){
                Matrix4<float> translation;
                translation = translation.Translation(12.0f * Vec3<float>(sinf(0.01f * getFrameCount()) * j, sinf(0.01f * getFrameCount()) * k, 0.0f));
                Matrix4<float> mvp = orientationAdjustedPerspective * rotation * translation;

                glUniformMatrix4fv(
                        glGetUniformLocation(_mProgram, "mvp"),
                        1,
                        GL_FALSE,
                        (GLfloat*)&mvp);
                glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
                glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);
            }
        }
    }

    width = storeWidth;
    height = storeHeight;
    calculatePerspective(45.0f);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
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

int RenderToCubeMapView::initialize() {
    /*renderedTexture = genCubeMap(GL_RGB, squareSize, squareSize, GL_LINEAR, NULL);
    glGenFramebuffers(6, &frameBuffers[0]);
    glGenRenderbuffers(6, &depthAndStencilRenderBuffers[0]);
    for (int i = 0; i < 6; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndStencilRenderBuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, squareSize, squareSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndStencilRenderBuffers[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X, renderedTexture, 0);
    bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return frameBufferStatus != GL_FRAMEBUFFER_COMPLETE;*/
}