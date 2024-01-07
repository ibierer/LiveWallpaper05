//
// Created by Immanuel Bierer on 1/6/2024.
//

#include "SimpleNBodySimulationView.h"

using std::to_string;

SimpleNBodySimulationView::SimpleNBodySimulationView() : View() {
    cubeProgram = View::createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    //simulation.initialize(Computation::ComputationOptions::CPU);
    simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION;
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), 0);
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
}

SimpleNBodySimulationView::~SimpleNBodySimulationView(){
    glDeleteProgram(mProgram);
}

void SimpleNBodySimulationView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(cubeProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation.computeShader.gVBO);
    cubeVAO.drawArraysInstanced(SimpleNBodySimulation::COUNT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    simulation.simulate(false, false);
}
