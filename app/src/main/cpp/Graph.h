//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_GRAPH_H
#define LIVEWALLPAPER05_GRAPH_H


class Graph : public Wallpaper{
public:

    GLuint mProgram;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
    "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
    "out vec3 vNormal;\n"
    "uniform mat4 mvp;\n"
    "void main() {\n"
    "    gl_Position = mvp*vec4(pos, 1.0);\n"
    "    vNormal = normal;\n"
    "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "}\n";

    Graph();

    explicit Graph(const string& equation);

    ~Graph();

    void render() override;

    ImplicitGrapher implicitGrapher;

private:
};


#endif //LIVEWALLPAPER05_GRAPH_H
