//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;

GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    graphProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FRAGMENT_SHADER.c_str());
    cubeProgram = createVertexAndFragmentShaderProgram(CUBE_VERTEX_SHADER.c_str(), CUBE_FRAGMENT_SHADER.c_str());

    implicitGrapher = ImplicitGrapher(ivec3(29));
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;

    //simulation.initialize(Computation::ComputationOptions::CPU);
    simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::DEFAULT_INDEX_BUFFER_BINDING;

    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    // Prepare model-view-projection matrix
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    // Render graph
    glUseProgram(graphProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(graphProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

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
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
    glVertexAttribDivisor(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 1);
    for(int i = 0; i < SimpleNBodySimulation::PARTICLES_PER_CHUNK; i++){
        glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::cacheChunk), (const GLvoid*)(offsetof(Simulation::Particle, position) + sizeof(Simulation::Particle) * i));
        glVertexAttribPointer(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::cacheChunk), (const GLvoid*)(offsetof(Simulation::Particle, velocity) + sizeof(Simulation::Particle) * i));
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, cubeVAO.getNumVertices(), SimpleNBodySimulation::NUM_CACHE_CHUNKS);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glBindVertexArray(0);

    simulation.simulate(1, NO, NO);
}
