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
    glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, position));
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
            glUseProgram(cubeProgram);
            glUniformMatrix4fv(
                    glGetUniformLocation(cubeProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation.computeShader.gVBO);
            cubeVAO.drawArraysInstanced(SimpleNBodySimulation::COUNT);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            break;
    }

    simulation.simulate(1, NO, NO);
}
