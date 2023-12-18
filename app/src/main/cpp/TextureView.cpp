//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "TextureView.h"

TextureView::TextureView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    texture = Texture(Texture::DefaultImages::MANDELBROT);
}

TextureView::~TextureView(){
    glDeleteProgram(mProgram);
}

void TextureView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 3.0f * (val - 1.0f)));
    Matrix4<float> translation2;
    translation2 = translation2.Translation(Vec3<float>(-0.5f, -0.5f, 0.0f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
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
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}