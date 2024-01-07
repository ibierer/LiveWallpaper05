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
            "struct particle {\n"
            "    vec3 position;\n"
            "    vec3 velocity;\n"
            "};\n"
            "layout(packed, binding = " + std::to_string(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION) + ") buffer destBuffer{\n"
            "	 particle particles[" + std::to_string(COUNT) + "];\n"
            "} outBuffer;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos + outBuffer.particles[gl_InstanceID].position, 1.0);\n"
            "    vColor = vec4(\n"
            "            0.06125f * outBuffer.particles[gl_InstanceID].velocity.x + 0.5f,\n"
            "            -0.06125f * outBuffer.particles[gl_InstanceID].velocity.y + 0.5f,\n"
            "            -0.06125f * outBuffer.particles[gl_InstanceID].velocity.z + 0.5f,\n"
            "            1.0f\n"
            "    );\n"
            "}\n";

    const string FRAGMENT_SHADER =
            View::ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "in vec4 vColor;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            //"    outColor = color;\n"
            "    outColor = vColor;\n"
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
