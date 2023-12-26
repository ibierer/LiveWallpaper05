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

    Vertex* box[24] = {
            // FRONT
            {{-0.5f, -0.5f,  0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            // BACK
            {{-0.5f, -0.5f, -0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            // LEFT
            {{-0.5f, -0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},
            {{-0.5f, -0.5f, -0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            // RIGHT
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            // TOP
            {{-0.5f,  0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            // BOTTOM
            {{-0.5f, -0.5f,  0.5f}},
            {{-0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{ 0.5f, -0.5f, -0.5f}}
    };

    View();

    ~View();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint createProgram(const char* const vtxSrc, const char* const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspective(const float& maxViewDegrees);

    const string ES_VERSION = supportsES32() ? "#version 320 es\n" : "#version 310 es\n";

    static string jstringToString(JNIEnv *env, jstring jStr);

    void incrementFrameCount();

    int getFrameCount();

private:

    int framesRendered;

    const static bool supportsES32();

};


#endif //LIVEWALLPAPER05_VIEW_H
