//
// Created by camer on 2/29/2024.
//

#ifndef LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATIONVIEW_H
#define LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATIONVIEW_H


#include <string>

using std::string;
using std::to_string;

class LinearithmicNBodySimulationView : public View {

public:

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " + to_string(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION) +
            ") in vec3 offset;\n"
            "layout(location = " + to_string(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION) +
            ") in vec3 velocity;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos + offset, 1.0);\n"
            "    vColor = vec4(\n"
            "            0.06125f * velocity.x + 0.5f,\n"
            "            -0.06125f * velocity.y + 0.5f,\n"
            "            -0.06125f * velocity.z + 0.5f,\n"
            "            1.0f\n"
            "    );\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec4 vColor;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vColor;\n"
            "}\n";

    LinearithmicNBodySimulation simulation;

    VertexArrayObject cubeVAO;

    GLuint cubeProgram;

    LinearithmicNBodySimulationView();

    ~LinearithmicNBodySimulationView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_LINEARITHMICNBODYSIMULATIONVIEW_H
