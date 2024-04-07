//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_PICFLIPVIEW_H
#define LIVEWALLPAPER05_PICFLIPVIEW_H


#include "FlipFluid.h"

class PicFlipView : public View {
public:

    VertexArrayObject cubeVAO;

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    GLuint sphereMapProgram;

    bool referenceFrameRotates;

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

    void render() override;

    PicFlipView(const bool &referenceFrameRotates);

    ~PicFlipView();

    FlipFluid* fluid;

    void simulate(const vec3 &acceleration, const mat3<float> &incrementalRotationMatrix);

    void setupScene();

private:

    struct {

        const int width = 1000;

        const int height = 1000;

    } canvas;

    float simHeight;

    float cScale;

    float simWidth;

    float simDepth;

};


#endif //LIVEWALLPAPER05_PICFLIPVIEW_H
