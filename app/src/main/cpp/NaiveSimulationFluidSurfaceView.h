//
// Created by Immanuel Bierer on 1/15/2024.
//

#ifndef LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H
#define LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H


class NaiveSimulationFluidSurfaceView : public View{
public:

    GLuint graphNormalMapProgram;

    GLuint graphFluidSurfaceProgram;

    GLuint graphFluidSurfaceClipsSphereProgram;

    GLuint cubeProgram;

    GLuint sphereNormalMapProgram;

    GLuint sphereMapDoubleRefractionProgram;

    GLuint sphereMapProgram;

    VertexArrayObject cubeVAO;

    VertexArrayObject sphereVAO;

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    VertexArrayObject tilesVAO;

    FBO fbo;

    NaiveSimulation simulation;

    Sphere sphere;

    ImplicitGrapher implicitGrapher;

    bool fluidSurface;

    bool referenceFrameRotates;

    const string _VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const string _FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "uniform mat4 mvp;\n"
            "uniform mat4 viewTransformation;\n"
            "out vec3 direction;\n"
            "out vec3 vNormal;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    direction = (viewTransformation * vec4(pos, 1.0)).xyz;\n"
            "    vNormal = normal;\n"
            "}\n";

    const string TILES_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "out vec3 position;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    position = pos;\n"
            "}\n";

    const string TILES_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D image;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(image, position.xy); \n"
            "}\n";

    const string GRAPH_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vPosition;\n"
            "out vec3 vNormal;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "uniform mat4 viewTransformation;\n"
            "uniform mat3 normalMatrix;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    vPosition = pos;\n"
            "    vNormal = normalMatrix * normal;\n"
            "    direction = (viewTransformation * vec4(pos, 1.0)).xyz;\n"
            "}\n";

    const string GRAPH_NORMAL_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(0.5f * normalize(vNormal) + vec3(0.5f), 1.0f); \n"
            "}\n";

    const string GRAPH_FLUID_SURFACE_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION + +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "    vec4 reflectedColor = Texture(environmentTexture, reflectedRay);\n"
            "    if(reflectivity == 1.0f){\n"
            "        outColor = reflectedColor;\n"
            "    }else{\n"
            "        float mixRatio = reflectivity < 0.0f ? fresnel(dotNI) : reflectivity;\n"
            "        vec3 refractedRay = normalize(refract2(normalizedDirection, normalizedNormal, inverseIOR, dotNI));\n"
            "        vec4 refractedColor;\n"
            "        if(twoSidedRefraction == 1){\n"
            "            vec4 bufferColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "            vec3 normalizedSecondaryNormal = normalize(vec3(0.5f) - bufferColor.rgb);\n"
            "            float secondaryDotNI = dot(refractedRay, normalizedSecondaryNormal);\n"
            "            vec3 secondaryRefractedRay = normalize(refract2(refractedRay, normalizedSecondaryNormal, indexOfRefraction, secondaryDotNI));\n"
            "            refractedColor = Texture(environmentTexture, secondaryRefractedRay);\n"
            "        }else{\n"
            "            refractedColor = Texture(environmentTexture, refractedRay);\n"
            "        }\n"
            "        outColor = mix(refractedColor, reflectedColor, mixRatio);\n"
            //"        outColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "    }\n"
            "}\n";

    const string GRAPH_FLUID_SURFACE_CLIPS_SPHERE_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION + +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "    vec4 reflectedColor = Texture(environmentTexture, reflectedRay);\n"
            "    if(reflectivity == 1.0f){\n"
            "        outColor = reflectedColor;\n"
            "    }else{\n"
            "        float mixRatio = reflectivity < 0.0f ? fresnel(dotNI) : reflectivity;\n"
            "        vec3 refractedRay = normalize(refract2(normalizedDirection, normalizedNormal, inverseIOR, dotNI));\n"
            "        vec4 refractedColor;\n"
            "        if(twoSidedRefraction == 1){\n"
            "            vec4 bufferColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "            vec3 normalizedSecondaryNormal = normalize(vec3(0.5f) - bufferColor.rgb);\n"
            "            float secondaryDotNI = dot(refractedRay, normalizedSecondaryNormal);\n"
            "            vec3 secondaryRefractedRay = normalize(refract2(refractedRay, normalizedSecondaryNormal, indexOfRefraction, secondaryDotNI));\n"
            "            refractedColor = Texture(environmentTexture, secondaryRefractedRay);\n"
            "        }else{\n"
            "            refractedColor = Texture(environmentTexture, refractedRay);\n"
            "        }\n"
            "        outColor = mix(refractedColor, reflectedColor, mixRatio);\n"
            //"        outColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "    }\n"
            "}\n";

    const string SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            DOUBLE_REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    vec4 bufferColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "    if(bufferColor.a == 0.0f){\n"
            "        outColor = Texture(environmentTexture, direction);\n"
            "        return;\n"
            "    }else if(bufferColor.a < 0.75f){\n"
            "       vec3 normalizedDirection = normalize(direction);\n"
            "       vec3 normalizedNormal = normalize(vNormal);\n"
            "       float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "       vec4 reflectedColor = Texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "       vec4 refractedColor = Texture(environmentTexture, doubleRefract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "       outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "    }else{\n"
            "        vec3 normalizedDirection = normalize(direction);\n"
            "        vec3 normalizedNormal = normalize(vNormal);\n"
            "        float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "        vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "        vec4 reflectedColor = Texture(environmentTexture, reflectedRay);\n"
            "        if(reflectivity == 1.0f){\n"
            "            outColor = reflectedColor;\n"
            "        }else{\n"
            "            float mixRatio = reflectivity < 0.0f ? fresnel(dotNI) : reflectivity;\n"
            "            vec3 refractedRay = normalize(refract2(normalizedDirection, normalizedNormal, inverseIOR, dotNI));\n"
            "            vec4 refractedColor;\n"
            "            if(twoSidedRefraction == 1){\n"
            "                vec4 bufferColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "                vec3 normalizedSecondaryNormal = normalize(vec3(0.5f) - bufferColor.rgb);\n"
            "                float secondaryDotNI = dot(refractedRay, normalizedSecondaryNormal);\n"
            "                vec3 secondaryRefractedRay = normalize(refract2(refractedRay, normalizedSecondaryNormal, indexOfRefraction, secondaryDotNI));\n"
            "                refractedColor = Texture(environmentTexture, secondaryRefractedRay);\n"
            "            }else{\n"
            "                refractedColor = Texture(environmentTexture, refractedRay);\n"
            "            }\n"
            "            outColor = mix(refractedColor, reflectedColor, mixRatio);\n"
            //"            outColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "        }\n"
            "    }\n"
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

    const string SPHERE_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vNormal;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    vNormal = normal;\n"
            "}\n";

    const string SPHERE_NORMAL_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(0.5f * normalize(vNormal) + vec3(0.5f), 0.5f); \n"
            "}\n";

    const string SPHERE_DOUBLE_ANGLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    outColor = vec4(0.5f * normalize(vNormal) + vec3(0.5f), 1.0f); \n"
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
            "precision highp float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "}\n";

    NaiveSimulationFluidSurfaceView(const int &particleCount,
                                    const bool &fluidSurface,
                                    const int &graphSize,
                                    const float &sphereRadius,
                                    const bool &referenceFrameRotates,
                                    const bool &smoothSphereSurface);

    ~NaiveSimulationFluidSurfaceView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H
