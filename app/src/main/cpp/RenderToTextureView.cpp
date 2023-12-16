//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "RenderToTextureView.h"

RenderToTextureView::RenderToTextureView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mProgram2 = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER2.c_str());
    texture = Texture(Texture::MS_PAINT_COLORS);
    fbo.initialize(1024, 1024);
}

RenderToTextureView::~RenderToTextureView(){
    glDeleteProgram(mProgram);
}

void RenderToTextureView::render(){
    int storeWidth = width;
    width = 2048;
    int storeHeight = height;
    height = 2048;
    calculatePerspective();
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.frameBuffer);
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(backgroundColor.r + 0.5f, backgroundColor.g + 0.5f, backgroundColor.b + 0.5f, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (val - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(mProgram2);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram2, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());

    Vertex vertices[4] = {
            {vec3(0.0f, 0.0f, 0.0f)},
            {vec3(0.0f, 1.0f, 0.0f)},
            {vec3(1.0f, 0.0f, 0.0f)},
            {vec3(1.0f, 1.0f, 0.0f)}
    };
    uvec3 indices[2] = {
            uvec3(0, 2, 1),
            uvec3(1, 3, 2)
    };
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    //glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);






    width = storeWidth;
    height = storeHeight;
    calculatePerspective();
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindTexture(GL_TEXTURE_2D, fbo.renderedTexture);

    //glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    //glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}