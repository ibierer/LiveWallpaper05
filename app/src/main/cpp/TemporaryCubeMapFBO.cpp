//
// Created by Immanuel Bierer on 12/22/2023.
//

#include "TemporaryCubeMapFBO.h"

TemporaryCubeMapFBO::TemporaryCubeMapFBO() {

}

TemporaryCubeMapFBO::TemporaryCubeMapFBO(void* texture, const bool& addDepthBuffer, const bool& addStencilBuffer) : renderedTexture(texture) {
    initialize(addDepthBuffer, addStencilBuffer);
}

// Copy constructor
TemporaryCubeMapFBO::TemporaryCubeMapFBO(const TemporaryCubeMapFBO& other) {
    // Shallow copy: sharing OpenGL resources
    renderedTexture = other.renderedTexture;
    for(int i = 0; i < 6; i++){
        frameBuffers[i] = other.frameBuffers[i];
        depthAndOrStencilRenderBuffers[i] = other.depthAndOrStencilRenderBuffers[i];
    }
}

// Assignment operator
TemporaryCubeMapFBO& TemporaryCubeMapFBO::operator=(const TemporaryCubeMapFBO& other) {
    if (this != &other) { // Check for self-assignment
        // Shallow copy: sharing OpenGL resources
        renderedTexture = other.renderedTexture;
        for(int i = 0; i < 6; i++){
            frameBuffers[i] = other.frameBuffers[i];
            depthAndOrStencilRenderBuffers[i] = other.depthAndOrStencilRenderBuffers[i];
        }
    }
    return *this;
}

TemporaryCubeMapFBO::~TemporaryCubeMapFBO() {

}

int TemporaryCubeMapFBO::initialize(const bool& addDepthBuffer, const bool& addStencilBuffer) {
    glBindTexture(GL_TEXTURE_2D, getRenderedTexture<Texture>().getTextureId());

    for(int i = 0; i < 6; i++) {
        glGenFramebuffers(1, &frameBuffers[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_2D, getRenderedTexture<Texture>().getTextureId(), 0);
        if (addDepthBuffer || addStencilBuffer) {
            glGenRenderbuffers(1, &depthAndOrStencilRenderBuffers[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            if (addDepthBuffer && !addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                                      getRenderedTexture<Texture>().getWidth(),
                                      getRenderedTexture<Texture>().getHeight());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                          depthAndOrStencilRenderBuffers[i]);
            } else if (!addDepthBuffer && addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8,
                                      getRenderedTexture<Texture>().getWidth(),
                                      getRenderedTexture<Texture>().getHeight());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                          depthAndOrStencilRenderBuffers[i]);
            } else if (addDepthBuffer && addStencilBuffer) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                      getRenderedTexture<Texture>().getWidth(),
                                      getRenderedTexture<Texture>().getHeight());
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER, depthAndOrStencilRenderBuffers[i]);
            }
        }
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return frameBufferStatus == GL_FRAMEBUFFER_COMPLETE;
}

GLuint TemporaryCubeMapFBO::getWidth() {
    return getRenderedTexture<Texture>().getWidth();
}

GLuint TemporaryCubeMapFBO::getHeight() {
    return getRenderedTexture<Texture>().getHeight();
}

GLuint TemporaryCubeMapFBO::getFrameBuffer(const int &i) {
    return frameBuffers[i];
}

GLuint TemporaryCubeMapFBO::getRenderedTextureId() {
    return getRenderedTexture<Texture>().getTextureId();
}

template<class T>
T& TemporaryCubeMapFBO::getRenderedTexture() {
    return *((Texture*)renderedTexture);
}
