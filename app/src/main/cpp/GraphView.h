//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_GRAPHVIEW_H
#define LIVEWALLPAPER05_GRAPHVIEW_H


#include <string>
#include "View.h"
#include "ImplicitGrapher.h"

using std::string;
using std::to_string;

class GraphView : public View {

public:

    ImplicitGrapher implicitGrapher;

    const string GRAPH_VERTEX_SHADER =
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

    const string GRAPH_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(0.025f * vPosition + vec3(0.5), 1.0f); \n"
            "}\n";

    const string CUBE_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " + to_string(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION) + ") in vec3 offset;\n"
            "layout(location = " + to_string(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION) + ") in vec3 velocity;\n"
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

    const string CUBE_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec4 vColor;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vColor;\n"
            "}\n";

    const string SPHERE_MAP_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string SPHERE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "}\n";

    SimpleNBodySimulation simulation;

    VertexArrayObject cubeVAO;

    GLuint graphProgram;

    GLuint cubeProgram;

    GLuint sphereMapProgram;

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    GraphView();

    explicit GraphView(const string& equation);

    ~GraphView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_GRAPHVIEW_H
