//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_VIEW_H
#define LIVEWALLPAPER05_VIEW_H


#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <string>
#include "Vertex.h"
#include "VertexNormal.h"
#include "cyCodeBase-master/cyMatrix.h"

using std::string;
using cy::Matrix4;

class View {
public:

    GLuint mProgram;

    vec4 backgroundColor;

    EGLContext mEglContext;

    float zoom;

    int orientation;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    Matrix4<float> orientationAdjustedPerspective;

    Vertex triangleVertices[3] = {
            Vertex(vec3(1.0f, 0.0f, 0.0f)),
            Vertex(vec3(0.0f, 1.0f, 0.0f)),
            Vertex(vec3(1.0f, 0.0f, 1.0f))
    };

    VertexNormal triangleVerticesNormals[3] = {
            VertexNormal(vec3(1.0f, 0.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
            VertexNormal(vec3(0.0f, 1.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
            VertexNormal(vec3(1.0f, 0.0f, 1.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f)))
    };

    Vertex tileVertices[4] = {
            Vertex(vec3(0.0f, 0.0f, 0.0f)),
            Vertex(vec3(0.0f, 1.0f, 0.0f)),
            Vertex(vec3(1.0f, 0.0f, 0.0f)),
            Vertex(vec3(1.0f, 1.0f, 0.0f))
    };

    Vertex tilesVertices[10] = {
            Vertex(vec3(0.0f, 0.0f, 0.0f)),
            Vertex(vec3(0.0f, 1.0f, 0.0f)),
            Vertex(vec3(1.0f, 0.0f, 0.0f)),
            Vertex(vec3(1.0f, 1.0f, 0.0f)),
            Vertex(vec3(1.0f, 1.0f, 0.0f)),
            Vertex(vec3(0.0f, 0.0f, 1.0f)),
            Vertex(vec3(0.0f, 0.0f, 1.0f)),
            Vertex(vec3(0.0f, 1.0f, 1.0f)),
            Vertex(vec3(1.0f, 0.0f, 1.0f)),
            Vertex(vec3(1.0f, 1.0f, 1.0f))
    };

    View();

    ~View();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint createProgram(const char* const vtxSrc, const char* const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspectiveSetViewport(const float& maxViewDegrees);

    const string ES_VERSION = supportsES32() ? "#version 320 es\n" : "#version 310 es\n";

    static string jstringToString(JNIEnv *env, jstring jStr);

    void incrementFrameCount();

    int getFrameCount();

private:

    int framesRendered;

    const static bool supportsES32();

};


#endif //LIVEWALLPAPER05_VIEW_H
