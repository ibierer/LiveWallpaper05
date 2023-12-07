//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "Wallpaper.h"

Wallpaper::Wallpaper() : framesRendered(0), val(0), mEglContext(eglGetCurrentContext()){

}

Wallpaper::~Wallpaper(){
    if (eglGetCurrentContext() != mEglContext) return;
}

void Wallpaper::render(){

}

void Wallpaper::calculatePerspective() {
    // Compute the window aspect ratio
    float aspect = (float)width / (float)height;
    float maxViewAngle = 45.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float verticalScreenAngle = (aspect > 1.0f) ? maxViewAngle : 2.0f * tanf(atanf(toRadians(0.5f * maxViewAngle)) / aspect);
    perspective.SetPerspective(verticalScreenAngle, aspect, zNear, zFar);
}

string Wallpaper::jstringToString(JNIEnv *env, jstring jStr) {
    if (jStr == nullptr) {
        return ""; // Handle null jstring gracefully
    }

    const char *chars = env->GetStringUTFChars(jStr, nullptr);
    string result(chars);
    env->ReleaseStringUTFChars(jStr, chars);

    return result;
}

// returns true if a GL error occurred
bool Wallpaper::checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint Wallpaper::createShader(const GLenum& shaderType, const char* src) {
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

GLuint Wallpaper::createProgram(const char* const vtxSrc, const char* const fragSrc) {
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

const bool Wallpaper::supportsES32(){
    const GLenum& shaderType = GL_VERTEX_SHADER;
    const char* src =
            "#version 320 es\n"
            "void main(){\n"
            "}\n";
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        Wallpaper::checkGlError("glCreateShader");
        return false;
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
                if(std::string(infoLog).find("'320' : client/version number not supported") != std::string::npos){
                    ALOGW("Could not compile OpenGL ES shader version %s: %s. Compiling version 310 instead.",
                          "320", infoLog);
                    free(infoLog);
                    return false;
                }
                free(infoLog);
                return false;
            }
        }
        glDeleteShader(shader);
        return false;
    }

    return true;
}

void Wallpaper::printGlString(const char* const name, const GLenum& s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}
