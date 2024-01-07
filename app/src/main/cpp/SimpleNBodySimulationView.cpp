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

    simulation.simulate(false);
    glUseProgram(cubeProgram);
    for(int i = 0; i < numCacheChunks; i++){
        for(int j = 0; j < starsPerChunk && starsPerChunk * i + j < COUNT; j++){
            Matrix4<float> translation2;
            translation2.SetTranslation(Vec3<float>(
                    simulation.data->chunks[i].stars[j].position.x,
                    simulation.data->chunks[i].stars[j].position.y,
                    simulation.data->chunks[i].stars[j].position.z
            ));
            mvp = orientationAdjustedPerspective * translation * rotation * translation2;
            glUniformMatrix4fv(
                    glGetUniformLocation(cubeProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            cubeVAO.draw();
            //ALOGI("data->chunks[type].stars[j].position = %s\n", data->chunks[type].stars[j].position.str().c_str());
        }
    }
}
