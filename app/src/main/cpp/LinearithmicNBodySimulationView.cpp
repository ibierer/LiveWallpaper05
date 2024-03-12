//
// Created by camer on 2/29/2024.
//

#include "LinearithmicNBodySimulationView.h"

using std::to_string;

LinearithmicNBodySimulationView::LinearithmicNBodySimulationView() : View() {
    cubeProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());

    simulation.initialize(Computation::ComputationOptions::CPU);
    //simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::DEFAULT_INDEX_BUFFER_BINDING;

    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
}

LinearithmicNBodySimulationView::~LinearithmicNBodySimulationView(){
    glDeleteProgram(mProgram);
}

void LinearithmicNBodySimulationView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -120.0f * distanceToOrigin));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
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
    glBindVertexArray(cubeVAO.getArrayObjectId());
    glBindBuffer(GL_ARRAY_BUFFER, simulation.computeShader.gVBO);
    glEnableVertexAttribArray(LinearithmicNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(LinearithmicNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glVertexAttribDivisor(LinearithmicNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
    glVertexAttribDivisor(LinearithmicNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 1);
    for(int i = 0; i < 1; i++){
        glVertexAttribPointer(LinearithmicNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::Star), (const GLvoid*)(offsetof(Simulation::Star, position) + sizeof(Simulation::Star) * i));
        glVertexAttribPointer(LinearithmicNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::Star), (const GLvoid*)(offsetof(Simulation::Star, velocity) + sizeof(Simulation::Star) * i));
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, cubeVAO.getNumVertices(), LinearithmicNBodySimulation::COUNT);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(LinearithmicNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(LinearithmicNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glBindVertexArray(0);

    simulation.simulate(1, NO, NO);
}
