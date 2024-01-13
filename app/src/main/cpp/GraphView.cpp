//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;

GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    implicitGrapher = ImplicitGrapher(ivec3(29));
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;

    cubeProgram = View::createVertexAndFragmentShaderProgram(_VERTEX_SHADER.c_str(),_FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    //simulation.initialize(Computation::ComputationOptions::CPU);
    simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION;
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, position));
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    ImplicitGrapher::calculateSurfaceOnGPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    // Prepare model-view-projection matrix
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    // Render graph
    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
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
    glUseProgram(cubeProgram);
    switch(simulation.getComputationOption()){
        case Computation::ComputationOptions::CPU:
            /*for(int i = 0; i < SimpleNBodySimulation::NUM_CACHE_CHUNKS; i++){
                for(int j = 0; j < SimpleNBodySimulation::PARTICLES_PER_CHUNK && SimpleNBodySimulation::PARTICLES_PER_CHUNK * i + j < SimpleNBodySimulation::COUNT; j++){
                    Matrix4<float> translation2;
                    translation2.SetTranslation(Vec3<float>(
                            simulation.data->chunks[i].particles[j].position.x,
                            simulation.data->chunks[i].particles[j].position.y,
                            simulation.data->chunks[i].particles[j].position.z
                    ));
                    mvp = orientationAdjustedPerspective * translation * rotation * translation2;
                    glUniformMatrix4fv(
                            glGetUniformLocation(cubeProgram, "mvp"),
                            1,
                            GL_FALSE,
                            (GLfloat*)&mvp);
                    cubeVAO.drawArrays();
                    //ALOGI("data->chunks[i].particles[j].position = %s\n", simulation.data->chunks[i].particles[j].position.str().c_str());
                }
            }*/
            /*for(int i = 0; i < SimpleNBodySimulation::COUNT; i++){
                Matrix4<float> translation2;
                translation2.SetTranslation(Vec3<float>(
                        simulation.data->particles[i].position.x,
                        simulation.data->particles[i].position.y,
                        simulation.data->particles[i].position.z
                ));
                mvp = orientationAdjustedPerspective * translation * rotation * translation2;
                glUniformMatrix4fv(
                        glGetUniformLocation(cubeProgram, "mvp"),
                        1,
                        GL_FALSE,
                        (GLfloat*)&mvp);
                cubeVAO.drawArrays();
                //ALOGI("data->particles[i].position = %s\n", simulation.data->particles[i].position.str().c_str());
            }
            break;*/
            simulation.pushData2GPU();
        case Computation::ComputationOptions::GPU:
            glUniformMatrix4fv(
                    glGetUniformLocation(cubeProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation.computeShader.gVBO);
            //cubeVAO.drawArraysInstanced(SimpleNBodySimulation::COUNT);
            glBindVertexArray(cubeVAO.getArrayObjectId());
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, cubeVAO.getNumVertices(), SimpleNBodySimulation::COUNT);
            glBindVertexArray(0);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            break;
    }

    simulation.simulate(1, NO, NO);
}
