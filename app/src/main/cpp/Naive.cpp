//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "Naive.h"

using std::min;
using std::max;

Naive::Naive() : Simulation(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, v));

    seed(15.0f);
}

Naive::~Naive(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
    delete molecules;
}

void Naive::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    for(int i = 0; i < moleculeCount; i++) {
        translation = translation.Translation(
                Vec3<float>(molecules[i].position[0], molecules[i].position[1], molecules[i].position[2] + 50.0f * (val - 1.0f)));
        Matrix4<float> mvp = perspective * translation;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.06125f * molecules[i].velocity.x + 0.5f,
                -0.06125f * molecules[i].velocity.y + 0.5f,
                -0.06125f * molecules[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    for(int i = 0; i < 5; i++){
        simulate(compensateForOrientation(accelerometerVector));
    }

    checkGlError("Renderer::render");
    framesRendered++;
}

void Naive::populateGrid(const int& dataIndex){
    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
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

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }
}

void Naive::useGrid(const int& i, const int& dataIndex, int* const ids, int& count){
    count = 0;
    float px = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
    float py = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
    float pz = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

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

bool Naive::seed(const float& radius) {
    setMoleculeCount(1000);
    molecules = new Molecule[moleculeCount];
    sphereRadius = radius;
    sphereRadiusSquared = sphereRadius * sphereRadius;
    sphereRadiusPlusPointFive = sphereRadius + 0.5f;
    sphereRadiusPlusPointFiveSquared = sphereRadiusPlusPointFive * sphereRadiusPlusPointFive;
    for (int i = 0; i < moleculeCount; i++) {
        int first = i - (i % (cbrtMoleculeCount * cbrtMoleculeCount));
        molecules[i].position.x = first / (cbrtMoleculeCount * cbrtMoleculeCount) - (cbrtMoleculeCount - 1) / 2.0f;
        first = i - first;
        molecules[i].position.y = (first - (first % cbrtMoleculeCount)) / cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].position.z = first % cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].velocity = vec3(0.0f, 0.000001f, 0.0f);
        molecules[i].positionFinal = molecules[i].position;
    }

    particleRadius = 0.5f;
    pInvSpacing = 1.0 / (maxForceDistance * 2.0 * particleRadius);//4.0 correlates to max distance of 2.0, 3.0 correlates to max distance of 1.5
    float width = 2.0f * sphereRadiusPlusPointFive;
    float height = 2.0f * sphereRadiusPlusPointFive;
    float depth = 2.0f * sphereRadiusPlusPointFive;
    pNumX = floor(width * pInvSpacing) + 1;
    pNumY = floor(height * pInvSpacing) + 1;
    pNumZ = floor(depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;
    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(moleculeCount, sizeof(int));

    populateGrid(0);

    return true;
}

float Naive::noPullBarrier(const float& x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    return 1000.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
}

float Naive::f(const float& x) {
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

inline void Naive::calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex){
    force = gravity;
    int ids[1000]; // 700 may be high enough
    int count = 0;
    useGrid(i, dataIndex, ids, count);
    for (int j = 0; j < count; j++) {
        if (i == j) {
            continue;
        }
        vec3 difference = ((vec3*)&molecules[ids[j]])[dataIndex] - position;
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

void Naive::simulate(const vec3& gravity) {
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceInitial, molecules[i].position, 0);
        molecules[i].positionFinal = molecules[i].position + deltaTime * (molecules[i].sumForceInitial * 0.5f * deltaTime + molecules[i].velocity);
        // Bug fix: Stop runaway particles
        float squaredDistance = dot(molecules[i].positionFinal, molecules[i].positionFinal);
        if(squaredDistance > sphereRadiusPlusPointFive * sphereRadiusPlusPointFive){
            molecules[i].positionFinal *= sqrt(sphereRadius * sphereRadius / squaredDistance);
        }
    }
    populateGrid(2);
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceFinal, molecules[i].positionFinal, 2);
        vec3 sumForceAverage = 0.5f * (molecules[i].sumForceInitial + molecules[i].sumForceFinal);
        molecules[i].position += deltaTime * (sumForceAverage * 0.5f * deltaTime + molecules[i].velocity);
        molecules[i].velocity += sumForceAverage * deltaTime;
        // Efficiency control
        if (efficiency != 1.0f) {
            vec3 product = molecules[i].velocity * efficiency;
            molecules[i].velocity = product;
        }
        //  Bug fix: stop runaway particles
        float squaredDistance = dot(molecules[i].position, molecules[i].position);
        if(squaredDistance > sphereRadiusSquared){
            molecules[i].position *= sqrt(sphereRadiusSquared / squaredDistance);
            molecules[i].velocity = 0.1f * molecules[i].position;
        }
    }
    populateGrid(0);
}