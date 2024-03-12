//
// Created by Immanuel Bierer on 12/17/2023.
//

#include "DrawWithFragmentShaderView.h"

DrawWithFragmentShaderView::DrawWithFragmentShaderView() : View() {
    tilesVAO = VertexArrayObject(View::tilesVertices, sizeof(View::tilesVertices) / sizeof(PositionXYZ));
    texture = Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this);
    fbo = FBO(&texture, NO, NO);
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
}

DrawWithFragmentShaderView::~DrawWithFragmentShaderView(){
    glDeleteProgram(mProgram);
}

void DrawWithFragmentShaderView::render(){

    // Option to regenerate the texture each frame.
    if(getFrameCount() > 0){
        //texture = Texture::dynamicallyGenerateMandelbrotWithVertexShader(fbo, this);
    }

    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -6.0f * distanceToOrigin));
    Matrix4<float> translation2;
    translation2 = translation2.Translation(Vec3<float>(-0.5f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "image"), 1);
    tilesVAO.drawArrays();
    glBindTexture(GL_TEXTURE_2D, 0);
}
