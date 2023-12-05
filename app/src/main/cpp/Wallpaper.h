//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_WALLPAPER_H
#define LIVEWALLPAPER05_WALLPAPER_H


class Wallpaper{
public:

    int framesRendered;

    vec4 backgroundColor;

    EGLContext mEglContext;

    float val;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    const Vertex BOX[24] = {
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

    Wallpaper();

    ~Wallpaper();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint createProgram(const char* const vtxSrc, const char* const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspective();

    const string ES_VERSION = supportsES32() ? "#version 320 es\n" : "#version 310 es\n";

    static string jstringToString(JNIEnv *env, jstring jStr) {
        if (jStr == nullptr) {
            return ""; // Handle null jstring gracefully
        }

        const char *chars = env->GetStringUTFChars(jStr, nullptr);
        string result(chars);
        env->ReleaseStringUTFChars(jStr, chars);

        return result;
    }

private:

    const static bool supportsES32();
};


#endif //LIVEWALLPAPER05_WALLPAPER_H
