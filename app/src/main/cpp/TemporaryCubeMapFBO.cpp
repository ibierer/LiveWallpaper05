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
    frameBuffer = other.frameBuffer;
    renderedTexture = other.renderedTexture;
    depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
}

// Assignment operator
TemporaryCubeMapFBO& TemporaryCubeMapFBO::operator=(const TemporaryCubeMapFBO& other) {
    if (this != &other) { // Check for self-assignment
        // Shallow copy: sharing OpenGL resources
        frameBuffer = other.frameBuffer;
        renderedTexture = other.renderedTexture;
        depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
    }
    return *this;
}

TemporaryCubeMapFBO::~TemporaryCubeMapFBO() {

}

int TemporaryCubeMapFBO::initialize(const bool& addDepthBuffer, const bool& addStencilBuffer) {
    glBindTexture(GL_TEXTURE_2D, getRenderedTexture<Texture>().getTextureId());
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_2D, getRenderedTexture<Texture>().getTextureId(), 0);

    if(addDepthBuffer || addStencilBuffer) {
        glGenRenderbuffers(1, &depthAndOrStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        if (addDepthBuffer && !addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, getRenderedTexture<Texture>().getWidth(), getRenderedTexture<Texture>().getHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (!addDepthBuffer && addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, getRenderedTexture<Texture>().getWidth(), getRenderedTexture<Texture>().getHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (addDepthBuffer && addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, getRenderedTexture<Texture>().getWidth(), getRenderedTexture<Texture>().getHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
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

GLuint TemporaryCubeMapFBO::getFrameBuffer() {
    return frameBuffer;
}

GLuint TemporaryCubeMapFBO::getRenderedTextureId() {
    return getRenderedTexture<Texture>().getTextureId();
}

template<class T>
T& TemporaryCubeMapFBO::getRenderedTexture() {
    return *((Texture*)renderedTexture);
}
