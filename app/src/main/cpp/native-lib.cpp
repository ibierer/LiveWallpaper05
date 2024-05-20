/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <string>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stack>
#include <android/log.h>
#include <cmath>
#include <EGL/egl.h>
#include "cyCodeBase-master/cyMatrix.h"

using cy::Matrix4;
using cy::Vec3;

#include <GLES3/gl32.h>
#define DEBUG 1

#define LOG_TAG "GLES3JNI"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#if DEBUG
#define ALOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif

#define STR(s) #s
#define STRV(s) STR(s)

#define POS_ATTRIB 0

static const char VERTEX_SHADER[] =
        "#version 310 es\n"
        "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
        "uniform mat4 mvp;\n"
        "uniform vec4 color;\n"
        "out vec4 vColor;\n"
        "void main() {\n"
        "    gl_Position = mvp * vec4(pos, 1.0);\n"
        "    vColor = color;\n"
        "}\n";

static const char FRAGMENT_SHADER[] =
        "#version 310 es\n"
        "precision mediump float;\n"
        "in vec4 vColor;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    outColor = vColor;\n"
        "}\n";

// This demo uses three coordinate spaces:
// - The model (a quad) is in a [-1 .. 1]^2 space
// - Scene space is either
//    landscape: [-1 .. 1] x [-1/(2*w/h) .. 1/(2*w/h)]
//    portrait:  [-1/(2*h/w) .. 1/(2*h/w)] x [-1 .. 1]
// - Clip space in OpenGL is [-1 .. 1]^2
//
// Conceptually, the quads are rotated in model space, then scaled (uniformly)
// and translated to place them in scene space. Scene space is then
// non-uniformly scaled to clip space. In practice the transforms are combined
// so vertices go directly from model to clip space.

struct Vertex {
    GLfloat pos[3];
};
const Vertex BOX[] = {
        // Square with diagonal < 2 so that it fits in a [-1 .. 1]^2 square
        // regardless of rotation.
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

// returns true if a GL error occurred
bool checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint createShader(GLenum shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                ALOGE("Could not compile %s shader:\n%s\n",
                      shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint createProgram(const char* vtxSrc, const char* fragSrc) {
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) goto exit;

    fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) goto exit;

    program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        goto exit;
    }
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        ALOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                ALOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    exit:
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return program;
}

static void printGlString(const char* name, GLenum s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

Matrix4<float> transformation;
std::stack <Matrix4<float>> transformationStack;
GLuint mProgram;
void glColor4f(float r, float g, float b, float a){
    glUniform4f(glGetUniformLocation(mProgram, "color"), r, g, b, a);
}

class Renderer {

public:

    Renderer() : mEglContext(eglGetCurrentContext()), mVBState(0), framesRendered(0){
        mProgram = 0;
    };

    ~Renderer(){
        /* The destructor may be called after the context has already been
         * destroyed, in which case our objects have already been destroyed.
         *
         * If the context exists, it must be current. This only happens when we're
         * cleaning up after a failed init().
         */
        if (eglGetCurrentContext() != mEglContext) return;
        glDeleteVertexArrays(1, &mVBState);
        glDeleteBuffers(1, mVB);
        glDeleteProgram(mProgram);
    };

    void resize(int w, int h){
        screenWidth = w;
        screenHeight = h;
        glViewport(0, 0, w, h);
    };

    void render(){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnableVertexAttribArray(POS_ATTRIB);
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(mProgram);
        Matrix4<float> perspective;
        perspective.SetPerspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
        Matrix4<float> translation;
        translation = translation.Translation(Vec3<float>(Vec3<float>(0.0f, 0.0f, sinf(framesRendered / 10.0f) - 3.0f)));
        Matrix4<float> rotation;
        rotation = rotation.RotationY((float)framesRendered / 10.0f);
        Matrix4<float> MVP = perspective * translation * rotation;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat*)&MVP.cell);
        glBindVertexArray(mVBState);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        glDisableVertexAttribArray(POS_ATTRIB);

        checkGlError("Renderer::render");
        framesRendered++;
    };

    bool init(){
        mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
        if (!mProgram) return false;

        glGenBuffers(1, mVB);
        glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &mVBState);
        glBindVertexArray(mVBState);

        glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (const GLvoid*)offsetof(Vertex, pos));

        ALOGV("Using OpenGL ES 3.0 renderer");
        return true;
    };

    const EGLContext mEglContext;
    GLuint mVB[1];
    GLuint mVBState;
    int framesRendered = 0;
    int screenWidth = 0;
    int screenHeight = 0;
};

// ----------------------------------------------------------------------------

static Renderer* g_renderer = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz, jstring JSON) {
    if (g_renderer) {
        delete g_renderer;
        g_renderer = NULL;
    }

    printGlString("Version", GL_VERSION);
    printGlString("Vendor", GL_VENDOR);
    printGlString("Renderer", GL_RENDERER);
    printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        Renderer* renderer = new Renderer;
        if (!renderer->init()) {
            delete renderer;
            g_renderer = NULL;
        } else {
            g_renderer = renderer;
        }
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_resize(JNIEnv *env, jobject thiz, jint width, jint height, jint orientation) {
    if (g_renderer) {
        g_renderer->resize(width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat linear_acc_x, jfloat linear_acc_y, jfloat linear_acc_z, jfloat distance, jfloat field_of_view, jfloat gravity, jfloat efficiency, jboolean flip_normals) {
    if (g_renderer) {
        g_renderer->render();
    }
}