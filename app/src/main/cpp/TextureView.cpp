//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "TextureView.h"

TextureView::TextureView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    tileVAO = VertexArrayObject(tileVertices, sizeof(tileVertices) / sizeof(PositionXYZ));
    //texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    texture = Texture(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
}

TextureView::~TextureView(){
    glDeleteProgram(mProgram);
}

void TextureView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 3.0f * (zoom - 1.0f)));
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

    tileVAO.draw();
}