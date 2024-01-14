//
// Created by Immanuel Bierer on 1/6/2024.
//

#include "SimpleNBodySimulationView.h"

using std::to_string;

SimpleNBodySimulationView::SimpleNBodySimulationView() : View() {
    cubeProgram = View::createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());

    //simulation.initialize(Computation::ComputationOptions::CPU);
    simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION;

    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    glBindVertexArray(cubeVAO.getArrayObjectId());
    glBindBuffer(GL_ARRAY_BUFFER, simulation.computeShader.gVBO);
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, position));
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
    glEnableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, velocity));
    glVertexAttribDivisor(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
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

    // Render cubes
    if(simulation.getComputationOption() == Computation::ComputationOptions::CPU) {
        simulation.pushData2GPU();
    }
    glUseProgram(cubeProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    cubeVAO.drawArraysInstanced(SimpleNBodySimulation::COUNT);

    simulation.simulate(1, NO, NO);
}
