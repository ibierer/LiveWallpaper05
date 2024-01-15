//
// Created by Immanuel Bierer on 1/15/2024.
//

#ifndef LIVEWALLPAPER05_NAIVESIMULATIONWITHSURFACEVIEW_H
#define LIVEWALLPAPER05_NAIVESIMULATIONWITHSURFACEVIEW_H


class NaiveSimulationWithSurfaceView : public View{
public:

    VertexArrayObject cubeVAO;

    NaiveSimulation simulation;

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

    NaiveSimulationWithSurfaceView();

    ~NaiveSimulationWithSurfaceView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_NAIVESIMULATIONWITHSURFACEVIEW_H
