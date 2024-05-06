//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SHADERTOYVIEW_H
#define LIVEWALLPAPER05_SHADERTOYVIEW_H


#include "ShaderToyView.h"

class ShaderToyView : public View {
public:

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    GLuint sphereMapProgram;

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
            //ES_VERSION +
            //"precision mediump float;\n"
            //"uniform sampler2D environmentTexture;\n"
            //"in vec3 direction;\n"
            //"out vec4 outColor;\n" +
            //SPHERE_MAP_TEXTURE_FUNCTION +
            //"void main() {\n"
            //"    outColor = Texture(environmentTexture, direction);\n"
            //"}\n";
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "uniform vec2 iResolution; // Pass the resolution of your rendering surface\n"
            "uniform float iTime;      // Pass the elapsed time since the start of the application\n"
            "uniform vec2 iMouse;      // Pass the current mouse position\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main(){\n"
            "   float ratio = iResolution.x / iResolution.y;\n"
            "   vec2 uv = vec2(ratio, 1.0f) * (2.0f * gl_FragCoord.xy / iResolution.xy - 1.0f);\n"
            "   \n"
            "   vec3 n = vec3(uv, sqrt(1.0f - clamp(dot(uv, uv), 0.0f, 1.0f)));\n"
            "   \n"
            "   vec3 color = 0.5f + 0.5f * n;\n"
            "   color = mix(vec3(0.5f), color, smoothstep(1.01f, 1.0f, dot(uv, uv)));\n"
            "   outColor = vec4(color, 1.0f);\n"
            "}\n";

    ShaderToyView();

    ~ShaderToyView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SHADERTOYVIEW_H