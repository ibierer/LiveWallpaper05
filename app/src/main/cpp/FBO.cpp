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

Texture& FBO::dynamicallyGenerateMandelbrotWithVertexShader(FBO& fbo, View* view) {
    glViewport(0, 0, fbo.getWidth(), fbo.getHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    const string VERTEX_SHADER =
            view->ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            view->ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D image;\n"
            "uniform int WIDTH;\n"
            "uniform int HEIGHT;\n"
            "out vec4 outColor;\n"
            "\n"
            "vec3 fetchFromSpectrum(float value) {\n"
            "    float index = value - floor(value);\n"
            "    if(index < 1.0f / 6.0f){\n"
            "        return vec3(\n"
            "                1.0f,\n"
            "                6.0f * index,\n"
            "                0.0f);\n"
            "    }else if(index < 2.0f / 6.0f){\n"
            "        return vec3(\n"
            "                2.0f - 6.0f * index,\n"
            "                1.0f,\n"
            "                0.0f);\n"
            "    }else if(index < 3.0f / 6.0f){\n"
            "        return vec3(\n"
            "                0.0f,\n"
            "                1.0f,\n"
            "                6.0f * index - 2.0f);\n"
            "    }else if(index < 4.0f / 6.0f){\n"
            "        return vec3(\n"
            "                0.0f,\n"
            "                4.0f - 6.0f * index,\n"
            "                1.0f);\n"
            "    }else if(index < 5.0f / 6.0f){\n"
            "        return vec3(\n"
            "                6.0f * index - 4.0f,\n"
            "                0.0f,\n"
            "                1.0f);\n"
            "    }else{\n"
            "        return vec3(\n"
            "                1.0f,\n"
            "                0.0f,\n"
            "                6.0f * (1.0f - index));\n"
            "    }\n"
            "}\n"
            "void main() {\n"
            "   // Maximum number of iterations for Mandelbrot algorithm\n"
            "   const int MAX_ITERATIONS = 1536;\n"
            "   float width = float(WIDTH);\n"
            "   float height = float(HEIGHT);\n"
            "   \n"
            "   float y = gl_FragCoord.y;\n"
            "   float x = gl_FragCoord.x;\n"
            "   // Map pixel coordinates to the complex plane\n"
            "   float minX = -2.0;\n"
            "   float maxX = 2.0;\n"
            "   float minY = -1.5;\n"
            "   float maxY = 1.5;\n"
            "   float cr = minX + x * (maxX - minX) / (width - 1.0f);\n"
            "   float ci = minY + y * (maxY - minY) / (height - 1.0f);\n"
            "   float zr = 0.0f;\n"
            "   float zi = 0.0f;\n"
            "   \n"
            "   int iterations = 0;\n"
            "   \n"
            "   // Iterate to check if the point is in the Mandelbrot set\n"
            "   while (iterations < MAX_ITERATIONS && (zr * zr + zi * zi) < 4.0f) {\n"
            "       float temp = zr * zr - zi * zi + cr;\n"
            "       zi = 2.0 * zr * zi + ci;\n"
            "       zr = temp;\n"
            "       \n"
            "       iterations++;\n"
            "   }\n"
            "   \n"
            "   if(iterations == MAX_ITERATIONS){\n"
            "       outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
            "   }else{\n"
            "       outColor = vec4(fetchFromSpectrum(0.005f * float(iterations)), 1.0f);\n"
            "   }\n"
            "}\n";
    GLuint mProgram = View::createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());

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

Texture &FBO::staticallyGenerateMandelbrotWithVertexShader(const Texture& texture, View* view) {
    FBO fbo = FBO(texture, NO, NO);
    return FBO::dynamicallyGenerateMandelbrotWithVertexShader(fbo, view);
}
