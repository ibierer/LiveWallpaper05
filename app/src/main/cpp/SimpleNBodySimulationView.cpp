//
// Created by Immanuel Bierer on 1/6/2024.
//

#include "SimpleNBodySimulationView.h"

using std::to_string;

SimpleNBodySimulationView::SimpleNBodySimulationView() : View() {
    //simulation.initialize(Computation::ComputationOptions::CPU);
    simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION;
    cubeProgram = View::createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());
    cube = Cube(1.0f, Cube::ColorOption::SOLID);
    //cubeVAO = VertexArrayObject(cube);
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, cube.getNumVertices() * sizeof(PositionXYZ), cube.getVertices<PositionXYZ>(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZ), (const GLvoid*)offsetof(PositionXYZ, p));
    glBindBuffer(GL_ARRAY_BUFFER, simulation.computeShader.gVBO);
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, position));
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
    glEnableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Simulation::Particle), (const GLvoid*)offsetof(Simulation::Particle, velocity));
    glVertexAttribDivisor(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 1);
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
            glBindBuffer(GL_ARRAY_BUFFER, simulation.computeShader.gVBO);
            //cubeVAO.drawArraysInstanced(SimpleNBodySimulation::COUNT);
            glBindVertexArray(mVAO);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, cube.getNumVertices(), SimpleNBodySimulation::COUNT);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            break;
    }

    simulation.simulate(1, NO, NO);
}
