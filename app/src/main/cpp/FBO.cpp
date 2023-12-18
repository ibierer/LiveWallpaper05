//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "FBO.h"

FBO::FBO() {

}

FBO::FBO(const int& width, const int& height, const bool& includeDepthBuffer, const bool& includeStencilBuffer){
    initialize(width, height, includeDepthBuffer, includeStencilBuffer);
}

// Copy constructor
FBO::FBO(const FBO& other) {
    // Shallow copy: sharing OpenGL resources
    frameBuffer = other.frameBuffer;
    renderedTexture = other.renderedTexture;
    depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
    width = other.width;
    height = other.height;
}

// Assignment operator
FBO& FBO::operator=(const FBO& other) {
    if (this != &other) { // Check for self-assignment
        // Shallow copy: sharing OpenGL resources
        frameBuffer = other.frameBuffer;
        renderedTexture = other.renderedTexture;
        depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
        width = other.width;
        height = other.height;
    }
    return *this;
}

FBO::~FBO() {

}

int FBO::initialize(const int& width, const int& height, const bool& includeDepthBuffer, const bool& includeStencilBuffer) {
    this->width = width;
    this->height = height;
    renderedTexture = Texture::generateTexture(width, height);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_2D, renderedTexture, 0);

    if(includeDepthBuffer || includeStencilBuffer) {
        glGenRenderbuffers(1, &depthAndOrStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        if (includeDepthBuffer && !includeStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (!includeDepthBuffer && includeStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (includeDepthBuffer && includeStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return frameBufferStatus == GL_FRAMEBUFFER_COMPLETE;
}

GLuint FBO::getWidth() {
    return width;
}

GLuint FBO::getHeight() {
    return height;
}

GLuint FBO::getFrameBuffer() {
    return frameBuffer;
}

GLuint FBO::getRenderedTexture() {
    return renderedTexture;
}
