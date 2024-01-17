//
// Created by Immanuel Bierer on 1/15/2024.
//

#ifndef LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H
#define LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H


class NaiveSimulationFluidSurfaceView : public View{
public:

    GLuint graphProgram;

    GLuint cubeProgram;

    VertexArrayObject cubeVAO;

    NaiveSimulation simulation;

    ImplicitGrapher implicitGrapher;

    bool referenceFrameRotates = true;

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
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const string CUBE_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    NaiveSimulationFluidSurfaceView(const int& particleCount, const int& graphSize, const float& sphereRadius);

    ~NaiveSimulationFluidSurfaceView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H
