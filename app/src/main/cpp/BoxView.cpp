//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "BoxView.h"

BoxView::BoxView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());

    glGenBuffers(1, mVBO);
    glGenVertexArrays(1, &mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), &box[0], GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, v));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

BoxView::~BoxView(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, mVBO);
}

void BoxView::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, zoom - 2.0f));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindVertexArray(mVAO);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    glBindVertexArray(0);

    checkGlError("Renderer::render");
}
