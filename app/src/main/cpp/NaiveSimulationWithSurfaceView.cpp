//
// Created by Immanuel Bierer on 1/15/2024.
//

#include "NaiveSimulationWithSurfaceView.h"

using std::min;
using std::max;

NaiveSimulationWithSurfaceView::NaiveSimulationWithSurfaceView() : View() {
    cubeProgram = createVertexAndFragmentShaderProgram(CUBE_VERTEX_SHADER.c_str(), CUBE_FRAGMENT_SHADER.c_str());
    graphProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    implicitGrapher = ImplicitGrapher(ivec3(29));
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::surfaceEquation = 0;
    ImplicitGrapher::numOfEquationsInMemory++;

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
    glUseProgram(cubeProgram);
    Matrix4<float> translation;
    for(int i = 0; i < simulation.particleCount; i++) {
        translation = translation.Translation(
                Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z + 50.0f * (zoom - 1.0f)));
        Matrix4<float> mvp = perspective * translation;
        glUniformMatrix4fv(
                glGetUniformLocation(cubeProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        vec4 color = vec4(
                0.06125f * simulation.particles[i].velocity.x + 0.5f,
                -0.06125f * simulation.particles[i].velocity.y + 0.5f,
                -0.06125f * simulation.particles[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(cubeProgram, "color"), 1, color.v);
        cubeVAO.drawArrays();
    }

    ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    // Prepare model-view-projection matrix
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

    for(int i = 0; i < 5; i++){
        simulation.simulate(compensateForOrientation(accelerometerVector));
    }

    checkGlError("Renderer::render");
}
