//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;

SimpleNBodySimulation simulation;

VertexArrayObject cubeVAO;

const string VERTEX_SHADER =
        View::ES_VERSION +
        "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
        "uniform mat4 mvp;\n"
        "out vec4 vColor;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(pos, 1.0);\n"
        "}\n";

const string FRAGMENT_SHADER =
        View::ES_VERSION +
        "precision mediump float;\n"
        "uniform vec4 color;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = color;\n"
        "}\n";

GLuint cubeProgram;
















GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    implicitGrapher = ImplicitGrapher(ivec3(29));
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    ImplicitGrapher::surfaceEquation = ImplicitGrapher::numOfEquationsInMemory;
    ImplicitGrapher::memoryEquations[ImplicitGrapher::numOfEquationsInMemory][1] = equation;
    ImplicitGrapher::processEquation(ImplicitGrapher::numOfEquationsInMemory);
    ImplicitGrapher::numOfEquationsInMemory++;

    cubeProgram = View::createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(),FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    simulation.initialize(Computation::ComputationOptions::CPU);
    //simulation.initialize(Computation::ComputationOptions::GPU);
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    ImplicitGrapher::calculateSurfaceOnGPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, &ImplicitGrapher::vertices[0], ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

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
