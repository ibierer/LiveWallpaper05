//
// Created by Immanuel Bierer on 12/16/2023.
//

#include "FBO.h"

FBO::FBO() {

}

FBO::FBO(Texture texture, const bool& addDepthBuffer, const bool& addStencilBuffer) : renderedTexture(texture) {
    initialize(addDepthBuffer, addStencilBuffer);
}

// Copy constructor
FBO::FBO(const FBO& other) {
    // Shallow copy: sharing OpenGL resources
    frameBuffer = other.frameBuffer;
    renderedTexture = other.renderedTexture;
    depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
}

// Assignment operator
FBO& FBO::operator=(const FBO& other) {
    if (this != &other) { // Check for self-assignment
        // Shallow copy: sharing OpenGL resources
        frameBuffer = other.frameBuffer;
        renderedTexture = other.renderedTexture;
        depthAndOrStencilRenderBuffer = other.depthAndOrStencilRenderBuffer;
    }
    return *this;
}

FBO::~FBO() {

}

int FBO::initialize(const bool& addDepthBuffer, const bool& addStencilBuffer) {
    glBindTexture(GL_TEXTURE_2D, renderedTexture.getTextureId());
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_2D, renderedTexture.getTextureId(), 0);

    if(addDepthBuffer || addStencilBuffer) {
        glGenRenderbuffers(1, &depthAndOrStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        if (addDepthBuffer && !addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderedTexture.getWidth(), renderedTexture.getHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (!addDepthBuffer && addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, renderedTexture.getWidth(), renderedTexture.getHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAndOrStencilRenderBuffer);
        } else if (addDepthBuffer && addStencilBuffer) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderedTexture.getWidth(), renderedTexture.getHeight());
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
    return renderedTexture.getWidth();
}

GLuint FBO::getHeight() {
    return renderedTexture.getHeight();
}

GLuint FBO::getFrameBuffer() {
    return frameBuffer;
}

GLuint FBO::getRenderedTexture() {
    return renderedTexture.getTextureId();
}

Texture& FBO::generateMandelbrotWithVertexShader(FBO& fbo, const GLuint& mProgram, View* view) {
    glViewport(0, 0, fbo.getWidth(), fbo.getHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glUseProgram(mProgram);
    glUniform1i(glGetUniformLocation(mProgram, "image"), 0);
    glUniform1i(glGetUniformLocation(mProgram, "WIDTH"), fbo.getWidth());
    glUniform1i(glGetUniformLocation(mProgram, "HEIGHT"), fbo.getHeight());

    Vertex vertices[4] = {
            {vec3(-1.0f, -1.0f, 0.0f)},
            {vec3(-1.0f, 1.0f, 0.0f)},
            {vec3(1.0f, -1.0f, 0.0f)},
            {vec3(1.0f, 1.0f, 0.0f)}
    };
    uvec3 indices[2] = {
            uvec3(0, 2, 1),
            uvec3(1, 3, 2)
    };

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) &vertices[0].v);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, view->width, view->height);

    return fbo.renderedTexture;
}
