//
// Created by Immanuel Bierer on 1/6/2024.
//

#ifndef LIVEWALLPAPER05_SIMPLENBODYSIMULATIONVIEW_H
#define LIVEWALLPAPER05_SIMPLENBODYSIMULATIONVIEW_H


#include <string>

using std::string;

class SimpleNBodySimulationView : public View {

public:

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

    SimpleNBodySimulation simulation;

    VertexArrayObject cubeVAO;

    GLuint cubeProgram;

    SimpleNBodySimulationView();

    ~SimpleNBodySimulationView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SIMPLENBODYSIMULATIONVIEW_H
