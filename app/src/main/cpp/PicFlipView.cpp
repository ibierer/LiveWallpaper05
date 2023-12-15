//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "PicFlipView.h"

PicFlipView::PicFlipView() : SimulationView(){
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

PicFlipView::~PicFlipView(){

}

void PicFlipView::simulate(const vec3& acceleration){
    fluid->simulate(
            fluid->dt, acceleration, fluid->flipRatio, fluid->numPressureIters, fluid->numParticleIters,
            fluid->overRelaxation, fluid->compensateDrift, fluid->separateParticles);
    fluid->frameNr++;
}

void PicFlipView::setupScene(){

    simHeight = 3.0;
    cScale = canvas.height / simHeight;
    simWidth = canvas.width / cScale;
    simDepth = simHeight;  // Assuming the depth is the same as the height

    int res = 15;

    float tankHeight = 1.0f * simHeight;
    float tankWidth = 1.0f * simWidth;
    float tankDepth = 1.0f * simDepth;
    float h = tankHeight / res;
    float density = 1000.0f;

    float relWaterHeight = 1.0f;
    float relWaterWidth = 0.6f;
    float relWaterDepth = 1.0f;

    // dam break

    // compute number of particles

    float r = 0.3 * h;    // particle radius w.r.t. cell size
    float dx = 2.0 * r;
    float dy = sqrt(3.0) / 2.0 * dx;
    float dz = 2.0 * r;

    int numX = floor((relWaterWidth * tankWidth - 2.0 * h - 2.0 * r) / dx);
    int numY = floor((relWaterHeight * tankHeight - 2.0 * h - 2.0 * r) / dy);
    int numZ = floor((relWaterDepth * tankDepth - 2.0 * h - 2.0 * r) / dz);
    int maxParticles = numX * numY * numZ;

    // create fluid

    fluid = new FlipFluid(density, tankWidth, tankHeight, tankDepth, h, r, maxParticles);

    // create particles

    fluid->numParticles = numX * numY * numZ;
    int p = 0;
    for (int i = 0; i < numX; i++) {
        for (int j = 0; j < numY; j++) {
            for (int k = 0; k < numZ; k++) {
                fluid->particlePos[p++] = vec3(
                        h + r + dx * i + (j % 2 == 0 ? 0.0 : r),
                        h + r + dy * j,
                        h + dz * k
                );
            }
        }
    }

    // setup grid cells for tank

    int nx = fluid->fNumX;
    int ny = fluid->fNumY;
    int nz = fluid->fNumZ;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                float s = 1.0;    // fluid
                if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1)
                    s = 0.0;    // solid
                fluid->s[(i * ny + j) * nz + k] = s;
            }
        }
    }
}

void PicFlipView::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(
            Vec3<float>(0.0f, 0.0f, 50.0f * (val - 1.0f)) - Vec3<float>(15.f));
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
