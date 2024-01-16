//
// Created by Immanuel Bierer on 1/15/2024.
//

#include "NaiveSimulationFluidSurfaceView.h"

using std::min;
using std::max;

NaiveSimulation* sim;

float fOfXYZFluidSurface(vec3 _) {
    _ -= ImplicitGrapher::offset;

    //if (abs(_.x) > 6.99f || abs(_.y) > 6.99f || abs(_.z) > 6.99f) {
    if(dot(_, _) > 6.99f * 6.99f) {
        return -1.0f;
    }

    _ *= 5.5f / 7.0f;

    float px = _.x + sim->sphereRadiusPlusPointFive;
    float py = _.y + sim->sphereRadiusPlusPointFive;
    float pz = _.z + sim->sphereRadiusPlusPointFive;

    int pxi = floor(px * sim->pInvSpacing);
    int pyi = floor(py * sim->pInvSpacing);
    int pzi = floor(pz * sim->pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, sim->pNumX - 1);
    int y1 = min(pyi + 1, sim->pNumY - 1);
    int z1 = min(pzi + 1, sim->pNumZ - 1);

    float sum = 0.0f;

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * sim->pNumY * sim->pNumZ + yi * sim->pNumZ + zi;
                int first = sim->firstCellParticle[cellNr];
                int last = sim->firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = sim->cellParticleIds[j];
                    vec3 difference = sim->particles[id].position - _;
                    float distanceSquared = dot(difference, difference);
                    if(distanceSquared < sim->maxForceDistanceSquared) {
                        sum += 1.0f / distanceSquared;
                    }
                }
            }
        }
    }
    return sum - 2.0f;
}

NaiveSimulationFluidSurfaceView::NaiveSimulationFluidSurfaceView() : View() {
    cubeProgram = createVertexAndFragmentShaderProgram(CUBE_VERTEX_SHADER.c_str(), CUBE_FRAGMENT_SHADER.c_str());
    graphProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    implicitGrapher = ImplicitGrapher(ivec3(14));

    simulation.seed(5.0f);
    sim = &simulation;
}

NaiveSimulationFluidSurfaceView::~NaiveSimulationFluidSurfaceView(){
    glDeleteProgram(mProgram);
    delete simulation.particles;
}

void NaiveSimulationFluidSurfaceView::render(){
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

    //ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);
    ImplicitGrapher::calculateSurfaceOnCPU(fOfXYZFluidSurface, 0.1f * getFrameCount(), 10, ImplicitGrapher::defaultOffset, 3.0f / 7.0f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    // Prepare model-view-projection matrix
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 60.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> translation2;
    translation2 = translation2.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

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
