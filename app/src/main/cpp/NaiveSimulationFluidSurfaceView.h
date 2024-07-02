//
// Created by Immanuel Bierer on 1/15/2024.
//

#ifndef LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H
#define LIVEWALLPAPER05_NAIVESIMULATIONFLUIDSURFACEVIEW_H


using std::function;

class NaiveSimulationFluidSurfaceView : public View{
public:

    GLuint graphNormalMapProgram;

    GLuint graphFluidSurfaceProgram;

    GLuint graphFluidSurfaceClipsSphereProgram;

    GLuint cubeProgram;

    GLuint sphereNormalMapProgram;

    GLuint environmentMapDoubleRefractionProgram;

    GLuint environmentMapProgram;

    VertexArrayObject cubeVAO;

    VertexArrayObject sphereVAO;

    VertexArrayObject environmentTriangleVAO;

    EnvironmentMap environmentMap;

    GLuint environmentMapTextureTarget;

    VertexArrayObject tilesVAO;

    NaiveSimulation simulation;

    Sphere sphere;

    ImplicitGrapher implicitGrapher;

    PositionXYZNormalXYZ* vertices;

    uvec3* indices;

    uint numIndices;

    bool sphereClipsGraph;

    bool fluidSurface;

    bool referenceFrameRotates;

    const function<float(vec3 _)> fOfXYZFluidSurface = [this](vec3 _) {
        _ -= implicitGrapher.defaultOffset;

        if (sphereClipsGraph) {
            if (dot(_, _) > (implicitGrapher.defaultOffset.x - 0.01f) * (implicitGrapher.defaultOffset.x - 0.01f)) {
                return -1.0f;
            }
        } else {
            if (dot(_, _) > (implicitGrapher.defaultOffset.x - 0.5f) * (implicitGrapher.defaultOffset.x - 0.5f)) {
                return -1.0f;
            }
        }

        _ *= simulation.sphereRadiusPlusPointFive / implicitGrapher.defaultOffset.x;

        float px = _.x + simulation.sphereRadiusPlusPointFive;
        float py = _.y + simulation.sphereRadiusPlusPointFive;
        float pz = _.z + simulation.sphereRadiusPlusPointFive;

        int pxi = floor(px * simulation.pInvSpacing);
        int pyi = floor(py * simulation.pInvSpacing);
        int pzi = floor(pz * simulation.pInvSpacing);
        int x0 = max(pxi - 1, 0);
        int y0 = max(pyi - 1, 0);
        int z0 = max(pzi - 1, 0);
        int x1 = min(pxi + 1, simulation.pNumX - 1);
        int y1 = min(pyi + 1, simulation.pNumY - 1);
        int z1 = min(pzi + 1, simulation.pNumZ - 1);

        float sum = 0.0f;

        for (int xi = x0; xi <= x1; xi++) {
            for (int yi = y0; yi <= y1; yi++) {
                for (int zi = z0; zi <= z1; zi++) {
                    int cellNr = xi * simulation.pNumY * simulation.pNumZ + yi * simulation.pNumZ + zi;
                    int first = simulation.firstCellParticle[cellNr];
                    int last = simulation.firstCellParticle[cellNr + 1];
                    for (int j = first; j < last; j++) {
                        int id = simulation.cellParticleIds[j];
                        vec3 difference = simulation.particles[id].position - _;
                        float distanceSquared = dot(difference, difference);
                        if(distanceSquared < simulation.maxForceDistanceSquared) {
                            sum += 1.0f / distanceSquared;
                        }
                    }
                }
            }
        }
        return sum - 2.0f;
    };

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

    const string GRAPH_FLUID_SURFACE_SPHERE_MAP_FRAGMENT_SHADER =
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
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "    return;\n"
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

    const string GRAPH_FLUID_SURFACE_CUBE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION + +
            "void main() {\n"
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    outColor = texture(environmentTexture, direction);\n"
            "    return;\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "    vec4 reflectedColor = texture(environmentTexture, reflectedRay);\n"
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
            "            refractedColor = texture(environmentTexture, secondaryRefractedRay);\n"
            "        }else{\n"
            "            refractedColor = texture(environmentTexture, refractedRay);\n"
            "        }\n"
            "        outColor = mix(refractedColor, reflectedColor, mixRatio);\n"
            //"        outColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "    }\n"
            "}\n";

    const string GRAPH_FLUID_SURFACE_CLIPS_SPHERE_SPHERE_MAP_FRAGMENT_SHADER =
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
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "    return;\n"
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

    const string GRAPH_FLUID_SURFACE_CLIPS_SPHERE_CUBE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION + +
            "void main() {\n"
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    outColor = texture(environmentTexture, direction);\n"
            "    return;\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "    vec4 reflectedColor = texture(environmentTexture, reflectedRay);\n"
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
            "            refractedColor = texture(environmentTexture, secondaryRefractedRay);\n"
            "        }else{\n"
            "            refractedColor = texture(environmentTexture, refractedRay);\n"
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
            "    outColor = Texture(environmentTexture, direction);\n"
            "    return;\n"
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

    const string CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform int twoSidedRefraction;\n"
            "uniform float reflectivity;\n"
            "uniform float indexOfRefraction;\n"
            "uniform float inverseIOR;\n"
            "uniform float screenWidth;\n"
            "uniform float screenHeight;\n"
            "uniform sampler2D image;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vPosition;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            DOUBLE_REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
            "    outColor = texture(environmentTexture, direction);\n"
            "    return;\n"
            "    vec4 bufferColor = texture(image, gl_FragCoord.xy/vec2(screenWidth, screenHeight));\n"
            "    if(bufferColor.a == 0.0f){\n"
            "        outColor = texture(environmentTexture, direction);\n"
            "        return;\n"
            "    }else if(bufferColor.a < 0.75f){\n"
            "       vec3 normalizedDirection = normalize(direction);\n"
            "       vec3 normalizedNormal = normalize(vNormal);\n"
            "       float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "       vec4 reflectedColor = texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "       vec4 refractedColor = texture(environmentTexture, doubleRefract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "       outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "    }else{\n"
            "        vec3 normalizedDirection = normalize(direction);\n"
            "        vec3 normalizedNormal = normalize(vNormal);\n"
            "        float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "        vec3 reflectedRay = reflect2(normalizedDirection, normalizedNormal, dotNI);\n"
            "        vec4 reflectedColor = texture(environmentTexture, reflectedRay);\n"
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
            "                refractedColor = texture(environmentTexture, secondaryRefractedRay);\n"
            "            }else{\n"
            "                refractedColor = texture(environmentTexture, refractedRay);\n"
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
