//
// Created by Immanuel Bierer on 12/17/2023.
//

#include "DrawWithFragmentShaderView.h"

DrawWithFragmentShaderView::DrawWithFragmentShaderView() : View() {
    texture = Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this);
    fbo = FBO((void*) &texture, NO, NO);
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
}

DrawWithFragmentShaderView::~DrawWithFragmentShaderView(){
    glDeleteProgram(mProgram);
}

void DrawWithFragmentShaderView::render(){

    // Option to regenerate the texture each frame.
    if(getFrameCount() > 0){
        //texture = FBO::dynamicallyGenerateMandelbrotWithVertexShader(temporaryCubeMapFBO, this);
    }

    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
    glUniform1i(glGetUniformLocation(mProgram, "image"), 1);

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

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid *) &vertices[0].v);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT,
                   indices);

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glBindTexture(GL_TEXTURE_2D, 0);
}
