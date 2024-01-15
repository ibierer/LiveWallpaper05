//
// Created by Immanuel Bierer on 1/15/2024.
//

#include "NaiveSimulationWithSurfaceView.h"

using std::min;
using std::max;

NaiveSimulationWithSurfaceView::NaiveSimulationWithSurfaceView() : View() {
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    simulation.seed(15.0f);
}

NaiveSimulationWithSurfaceView::~NaiveSimulationWithSurfaceView(){
    glDeleteProgram(mProgram);
    delete simulation.particles;
}

void NaiveSimulationWithSurfaceView::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    for(int i = 0; i < simulation.particleCount; i++) {
        translation = translation.Translation(
                Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z + 50.0f * (zoom - 1.0f)));
        Matrix4<float> mvp = perspective * translation;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        vec4 color = vec4(
                0.06125f * simulation.particles[i].velocity.x + 0.5f,
                -0.06125f * simulation.particles[i].velocity.y + 0.5f,
                -0.06125f * simulation.particles[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        cubeVAO.drawArrays();
    }

    for(int i = 0; i < 5; i++){
        simulation.simulate(compensateForOrientation(accelerometerVector));
    }

    checkGlError("Renderer::render");
}
