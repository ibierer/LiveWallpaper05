//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_NAIVEVIEW_H
#define LIVEWALLPAPER05_NAIVEVIEW_H


#include "SimulationView.h"

class NaiveView : public SimulationView{
public:

    GLuint mVB[1];

    GLuint mVBState;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    struct Molecule {
        vec3 position;
        vec3 velocity;
        vec3 positionFinal;
        vec3 sumForceInitial;
        vec3 sumForceFinal;
    };

    Molecule* molecules;

    bool seed(const float& radius);

    void simulate(const vec3& gravity);

    NaiveView();

    ~NaiveView();

    void render() override;

private:

    const float deltaTime = 0.02f;

    const float surfaceTension = 0.05f;

    float sphereRadius;

    float sphereRadiusSquared;

    float sphereRadiusPlusPointFive;

    float sphereRadiusPlusPointFiveSquared;

    const float efficiency = 1.0f;//0.995f;

    const float maxForce = 100000000.0f;

    const float maxForceSquared = maxForce * maxForce;

    const double maxForceDistance = 2.0;

    const double maxForceDistanceSquared = maxForceDistance * maxForceDistance;

    float particleRadius;

    int* numCellParticles;

    int* firstCellParticle;

    int* cellParticleIds;

    float pInvSpacing;

    int pNumX;

    int pNumY;

    int pNumZ;

    int pNumCells;

    void populateGrid(const int& dataIndex);

    void useGrid(const int& i, const int& dataIndex, int* const ids, int& count);

    float noPullBarrier(const float& x);

    float f(const float& x);

    inline void calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex);
};


#endif //LIVEWALLPAPER05_NAIVEVIEW_H
