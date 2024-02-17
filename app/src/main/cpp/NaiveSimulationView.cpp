//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "NaiveSimulationView.h"

NaiveSimulationView::NaiveSimulationView(const int &particleCount, const float &sphereRadius, const bool &referenceFrameRotates, const bool &gravityOn) : View() {
    this->referenceFrameRotates = referenceFrameRotates;
    this->gravityOn = gravityOn;

    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    simulation.seed(particleCount, sphereRadius);
}

NaiveSimulationView::~NaiveSimulationView(){
    glDeleteProgram(mProgram);
    delete simulation.particles;
}

void NaiveSimulationView::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> mvp;
    Matrix4<float> projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
    Matrix4<float> view;
    Matrix4<float> model;
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * (zoom - 1.0f)));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    for(int i = 0; i < simulation.particleCount; i++) {
        model = model.Translation(Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z));
        view = referenceFrameRotates ? translation : translation * rotation;
        mvp = projection * view * model;
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
        float linearAccelerationMultiplier = 8.0f * (getFrameCount() > 10 ? 1.0f : 1.0f / 10 * getFrameCount());
        if(referenceFrameRotates){
            if(gravityOn) {
                simulation.simulate(compensateForOrientation(accelerometerVector));
            }else{
                simulation.simulate(compensateForOrientation(linearAccelerationMultiplier * linearAccelerationVector));
            }
        }else {
            if(gravityOn){
                simulation.simulate(quaternionTo3x3(rotationVector) * (-accelerometerVector));
            }else{
                simulation.simulate(quaternionTo3x3(rotationVector) * (-linearAccelerationMultiplier * linearAccelerationVector));
            }
        }
    }
}
