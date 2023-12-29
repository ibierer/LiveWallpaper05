//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "RenderToTextureView.h"

RenderToTextureView::RenderToTextureView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    tilesVAO = VertexArrayObject(tilesVertices, sizeof(tilesVertices) / sizeof(Vertex));
    texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    //texture = Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this);
    fbo = FBO(
            (void*) new Texture(GL_RGB, texture.getWidth(), texture.getHeight(), 0, GL_LINEAR),
            YES,
            NO);
}

RenderToTextureView::~RenderToTextureView(){
    glDeleteProgram(mProgram);
}

void RenderToTextureView::render(){
    int storeWidth = width;
    width = fbo.getWidth();
    int storeHeight = height;
    height = fbo.getHeight();
    calculatePerspectiveSetViewport(60.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(backgroundColor.r + 0.5f, backgroundColor.g + 0.5f, backgroundColor.b + 0.5f, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 3.0f * (zoom - 1.0f)));
    Matrix4<float> translation2;
    translation2 = translation2.Translation(Vec3<float>(-0.5f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "image"), 0);

    tilesVAO.draw();

    width = storeWidth;
    height = storeHeight;
    calculatePerspectiveSetViewport(60.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mvp = orientationAdjustedPerspective * translation * rotation * translation2;

    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "image"), 1);

    tilesVAO.draw();
}
