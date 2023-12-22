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
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

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
    /*for (int i = 0; i < 6; ++i) { // This breaks the depth and stencil buffers
        glDeleteFramebuffers(1, &frameBuffers[i]);
        glDeleteRenderbuffers(1, &depthAndOrStencilRenderBuffers[i]);
    }*/
}

int CubeMapFBO::getResolution() {
    return cubeMap.getResolution();
}

GLuint CubeMapFBO::getTextureId() {
    return cubeMap.getTextureId();
}
