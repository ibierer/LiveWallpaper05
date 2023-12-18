//
// Created by Immanuel Bierer on 12/17/2023.
//

#include "DrawWithFragmentShaderView.h"

DrawWithFragmentShaderView::DrawWithFragmentShaderView() : View() {
    fbo = FBO(Texture(Texture::generateTexture(1536, 1536), 1536, 1536), 1536, 1536, true, false);
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    mPlanesProgram = createProgram(PLANES_VERTEX_SHADER.c_str(), PLANES_FRAGMENT_SHADER.c_str());
    texture = Texture(Texture::MS_PAINT_COLORS);
}

DrawWithFragmentShaderView::~DrawWithFragmentShaderView(){
    glDeleteProgram(mProgram);
}

void DrawWithFragmentShaderView::render(){
    int storeWidth = width;
    width = fbo.getWidth();
    int storeHeight = height;
    height = fbo.getHeight();
    calculatePerspective(60.0f);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(backgroundColor.r + 0.5f, backgroundColor.g + 0.5f, backgroundColor.b + 0.5f, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glUseProgram(mProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "image"), 0);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    {
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
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (const GLvoid *) &vertices[0].v);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT,
                       indices);
    }

    width = storeWidth;
    height = storeHeight;
    calculatePerspective(60.0f);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 3.0f * (val - 1.0f)));
    Matrix4<float> translation2;
    translation2 = translation2.Translation(Vec3<float>(-0.5f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

    glUseProgram(mPlanesProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mPlanesProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTexture());
    glUniform1i(glGetUniformLocation(mPlanesProgram, "image"), 1);

    {
        Vertex vertices[8] = {
                {vec3(0.0f, 0.0f, 0.0f)},
                {vec3(0.0f, 1.0f, 0.0f)},
                {vec3(1.0f, 0.0f, 0.0f)},
                {vec3(1.0f, 1.0f, 0.0f)},
                {vec3(0.0f, 0.0f, 1.0f)},
                {vec3(0.0f, 1.0f, 1.0f)},
                {vec3(1.0f, 0.0f, 1.0f)},
                {vec3(1.0f, 1.0f, 1.0f)}
        };
        uvec3 indices[4] = {
                uvec3(0, 2, 1),
                uvec3(1, 3, 2),
                uvec3(4, 6, 5),
                uvec3(5, 7, 6)
        };
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (const GLvoid *) &vertices[0].v);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT,
                       indices);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}
