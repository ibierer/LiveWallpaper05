//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "NaiveSimulationView.h"

using std::min;
using std::max;

NaiveSimulationView::NaiveSimulationView() : View() {
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    simulation.seed(15.0f);
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
    Matrix4<float> translation;
    for(int i = 0; i < simulation.count; i++) {
        translation = translation.Translation(
                Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z + 50.0f * (zoom - 1.0f)));
        Matrix4<float> mvp = perspective * translation;
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
        simulation.simulate(compensateForOrientation(accelerometerVector));
    }

    checkGlError("Renderer::render");
}

void NaiveSimulation::populateGrid(const int& dataIndex){
    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < count; i++) {
        float x = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        numCellParticles[cellNr]++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += numCellParticles[i];
        firstCellParticle[i] = first;
    }
    firstCellParticle[pNumCells] = first; // guard

    // fill particles into cells

    for (int i = 0; i < count; i++) {
        float x = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0.0f, float(pNumX - 1));
        int yi = clamp(floor(y * pInvSpacing), 0.0f, float(pNumY - 1));
        int zi = clamp(floor(z * pInvSpacing), 0.0f, float(pNumZ - 1));
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }
}

void NaiveSimulation::useGrid(const int& i, const int& dataIndex, int* const ids, int& count){
    count = 0;
    float px = ((vec3*)&particles[i])[dataIndex].x + sphereRadiusPlusPointFive;
    float py = ((vec3*)&particles[i])[dataIndex].y + sphereRadiusPlusPointFive;
    float pz = ((vec3*)&particles[i])[dataIndex].z + sphereRadiusPlusPointFive;

    int pxi = floor(px * pInvSpacing);
    int pyi = floor(py * pInvSpacing);
    int pzi = floor(pz * pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, pNumX - 1);
    int y1 = min(pyi + 1, pNumY - 1);
    int z1 = min(pzi + 1, pNumZ - 1);

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
                int first = firstCellParticle[cellNr];
                int last = firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = cellParticleIds[j];
                    if (id == i)
                        continue;
                    ids[count++] = id;
                }
            }
        }
    }
}

float NaiveSimulation::noPullBarrier(const float& x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    return 1000.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
}

float NaiveSimulation::f(const float& x) {
    if (x > maxForceDistance) {
        return 0.0f;
    }
    float _return = 500.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
    if (x > 1.0f) {
        return surfaceTension * _return;
    } else {
        return _return;
    }
}

inline void NaiveSimulation::calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex){
    force = gravity;
    int ids[1000]; // 700 may be high enough
    int count = 0;
    useGrid(i, dataIndex, ids, count);
    for (int j = 0; j < count; j++) {
        if (i == j) {
            continue;
        }
        vec3 difference = ((vec3*)&particles[ids[j]])[dataIndex] - position;
        float r = distance(difference);
        float fOverR = f(r) / r;
        force += fOverR * difference;
    }
    float distanceFromOrigin = distance(position);
    if (distanceFromOrigin != 0.0f) {
        float forceAtPoint = noPullBarrier(-distanceFromOrigin + sphereRadiusPlusPointFive) / distanceFromOrigin;//if distanceFromOrigin = 0, then we plug in a value of 3.5, if distanceFromOrigin = 2.5, then we plug in a value of 1
        force += forceAtPoint * position;
    }
}
