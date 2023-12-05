//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "PicFlip.h"

PicFlip::PicFlip() : Simulation(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, v));

    setupScene();
}

PicFlip::~PicFlip(){

}

void PicFlip::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(
            Vec3<float>(0.0f, 0.0f, val * 100.0f - 60.0f) - Vec3<float>(15.f));
    Matrix4<float> translation2;
    for (int i = 0; i < fluid->numParticles; i++) {
        translation2 = translation2.Translation(
                10.0f * Vec3<float>(fluid->particlePos[i].x, fluid->particlePos[i].y,
                                    fluid->particlePos[i].z));
        Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
        Matrix4<float> mvp = perspective * translation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.5f * fluid->particleVel[i].x + 0.5f,
                -0.5f * fluid->particleVel[i].y + 0.5f,
                -0.5f * fluid->particleVel[i].z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    for (int i = 0; i < 2; i++) {
        if (distance(accelerometerVector) == 0.0f) {
            simulate(vec3(0.0f, -9.81f, 0.0f));
        } else {
            simulate(compensateForOrientation(accelerometerVector));
        }
    }

    checkGlError("Renderer::render");
    framesRendered++;
}
