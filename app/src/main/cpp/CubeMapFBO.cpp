//
// Created by Immanuel Bierer on 12/17/2023.
//

#include "CubeMapFBO.h"

CubeMapFBO::CubeMapFBO() {

}

CubeMapFBO::CubeMapFBO(CubeMap cubeMap, const bool &addDepthBuffer, const bool &addStencilBuffer) {
    initialize(cubeMap, addDepthBuffer, addStencilBuffer);
}

CubeMapFBO::CubeMapFBO(const CubeMapFBO& other) : cubeMap(other.cubeMap) {
    // Assign the CubeMap
    cubeMap = other.cubeMap;
    // Copy the frameBuffers and depthAndStencilRenderBuffers
    for (int i = 0; i < 6; ++i) {
        frameBuffers[i] = other.frameBuffers[i];
        depthAndOrStencilRenderBuffers[i] = other.depthAndOrStencilRenderBuffers[i];
    }
}

bool CubeMapFBO::initialize(CubeMap cubeMap, const bool &addDepthBuffer, const bool &addStencilBuffer){
    this->cubeMap = cubeMap;

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getTextureId());
    glGenFramebuffers(6, frameBuffers);
    if(addDepthBuffer || addStencilBuffer){
        for (int i = 0; i < 6; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
            glGenRenderbuffers(1, &depthAndOrStencilRenderBuffers[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            if (addDepthBuffer && !addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMap.getResolution(), cubeMap.getResolution());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            } else if (!addDepthBuffer && addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, cubeMap.getResolution(), cubeMap.getResolution());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            } else if (addDepthBuffer && addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, cubeMap.getResolution(), cubeMap.getResolution());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeMap.getTextureId(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return frameBufferStatus != GL_FRAMEBUFFER_COMPLETE;
}

CubeMapFBO &CubeMapFBO::operator=(const CubeMapFBO &other) {
    // Check for self-assignment
    if (this != &other) {
        // Assign the CubeMap
        cubeMap = other.cubeMap;

        // Copy the frameBuffers and depthAndStencilRenderBuffers
        for (int i = 0; i < 6; ++i) {
            frameBuffers[i] = other.frameBuffers[i];
            depthAndOrStencilRenderBuffers[i] = other.depthAndOrStencilRenderBuffers[i];
        }
    }

    return *this;
}

CubeMapFBO::~CubeMapFBO() {
    // Delete OpenGL resources in the destructor
    for (int i = 0; i < 6; ++i) {
        glDeleteFramebuffers(1, &frameBuffers[i]);
        glDeleteRenderbuffers(1, &depthAndOrStencilRenderBuffers[i]);
    }
}

int CubeMapFBO::getResolution() {
    return cubeMap.getResolution();
}

GLuint CubeMapFBO::getTextureId() {
    return cubeMap.getTextureId();
}
