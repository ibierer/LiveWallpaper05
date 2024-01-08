//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_GRAPHVIEW_H
#define LIVEWALLPAPER05_GRAPHVIEW_H


#include <string>
#include "View.h"
#include "ImplicitGrapher.h"

using std::string;

class GraphView : public View {

public:

    ImplicitGrapher implicitGrapher;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vPosition;\n"
            "out vec3 vNormal;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    vPosition = pos;\n"
            "    vNormal = normal;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(0.025f * vPosition + vec3(0.5), 1.0f); \n"
            "}\n";

    const string _VERTEX_SHADER =
            View::ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "struct particle {\n"
            "    vec3 position;\n"
            "    vec3 velocity;\n"
            "};\n"
            "layout(packed, binding = " + std::to_string(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION) + ") buffer destBuffer{\n"
            "	 particle particles[" + std::to_string(SimpleNBodySimulation::COUNT) + "];\n"
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

    const string _FRAGMENT_SHADER =
            View::ES_VERSION +
            "precision mediump float;\n"
            "in vec4 vColor;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vColor;\n"
            "}\n";

    SimpleNBodySimulation simulation;

    VertexArrayObject cubeVAO;

    GLuint cubeProgram;

    GraphView();

    explicit GraphView(const string& equation);

    ~GraphView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_GRAPHVIEW_H
