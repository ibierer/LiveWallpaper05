//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "View.h"

const PositionXYZ View::triangleVertices[3] = {
        PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
        PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 0.0f, 1.0f))
};

const PositionXYZNormalXYZ View::triangleVerticesNormals[3] = {
        PositionXYZNormalXYZ(vec3(1.0f, 0.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
        PositionXYZNormalXYZ(vec3(0.0f, 1.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
        PositionXYZNormalXYZ(vec3(1.0f, 0.0f, 1.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f)))
};

const PositionXYZ View::tileVertices[4] = {
        PositionXYZ(vec3(0.0f, 0.0f, 0.0f)),
        PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 1.0f, 0.0f))
};

const PositionXYZ View::tilesVertices[10] = {
        PositionXYZ(vec3(0.0f, 0.0f, 0.0f)),
        PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 1.0f, 0.0f)),
        PositionXYZ(vec3(1.0f, 1.0f, 0.0f)),
        PositionXYZ(vec3(0.0f, 0.0f, 1.0f)),
        PositionXYZ(vec3(0.0f, 0.0f, 1.0f)),
        PositionXYZ(vec3(0.0f, 1.0f, 1.0f)),
        PositionXYZ(vec3(1.0f, 0.0f, 1.0f)),
        PositionXYZ(vec3(1.0f, 1.0f, 1.0f))
};

const string View::ES_VERSION = supportsES32() ? "#version 320 es\n" : "#version 310 es\n";

const string View::SPHERE_MAP_TEXTURE_FUNCTION =
        "vec4 Texture(sampler2D sampler, vec3 direction) {\n"
        // Normalize the direction vector
        "    vec3 normalizedDirection = normalize(direction);\n"
        // Calculate polar angle (theta) and azimuthal angle (phi)
        "    float theta = acos(normalizedDirection.z); // polar angle\n"
        "    float phi = atan(normalizedDirection.y, normalizedDirection.x); // azimuthal angle\n"
        // Map angles to UV coordinates
        "    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
        "    float p = 1.0 - theta / 3.14159265359;\n"
        "    return texture(sampler, vec2(u, p));\n"
        "}\n";

const string View::REFLECT2_FUNCTION =
        "vec3 reflect2(vec3 I, vec3 N, float dotNI) {\n"
        "    return I - 2.0 * dotNI * N;\n"
        "}\n";

const string View::REFRACT2_FUNCTION =
        "vec3 refract2(vec3 I, vec3 N, float eta, float dotNI) {\n"
        "    vec3 R;\n"
        "    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);\n"
        "    if (k < 0.0)\n"
        "        return(R = vec3(0.0));\n"
        "    else\n"
        "        R = eta * I - (eta * dotNI + sqrt(k)) * N;\n"
        "    return R;\n"
        "}\n";

const string View::DOUBLE_REFRACT_FUNCTION =
        "vec3 doubleRefract(vec3 I, vec3 N, float eta) {\n"
        "    vec3 R;\n"
        "    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));\n"
        "    if (k < 0.0)\n"
        "        return(R = vec3(0.0));\n"
        "    else\n"
        "        R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;\n"
        "    return 2.0f * dot(I, R) * R - I;\n"
        "}\n";

const string View::DOUBLE_REFRACT2_FUNCTION =
        "vec3 doubleRefract2(vec3 I, vec3 N, float eta, float dotNI) {\n"
        "    vec3 R;\n"
        "    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);\n"
        "    if (k < 0.0)\n"
        "        return(R = vec3(0.0));\n"
        "    else\n"
        "        R = eta * I - (eta * dotNI + sqrt(k)) * N;\n"
        "    return 2.0f * dot(I, R) * R - I;\n"
        "}\n";

const string View::FRESNEL_EFFECT_FUNCTION =
        "float fresnel(float dotNI){ \n"
        "    return pow(1.0 + dotNI, 3.0); \n"
        "} \n";

View::View() : framesRendered(0), zoom(0), mEglContext(eglGetCurrentContext()){

}

View::~View(){
    if (eglGetCurrentContext() != mEglContext) return;
}

void View::render(){

}

void View::calculatePerspectiveSetViewport(const float& maxViewDegrees) {
    float aspect = (float)width / (float)height;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float verticalScreenAngle = aspect < 1.0f ? toRadians(maxViewDegrees) : 2.0f * atanf(tanf(0.5f * toRadians(maxViewDegrees)) / aspect);
    perspective.SetPerspective(verticalScreenAngle, aspect, zNear, zFar);
    Matrix4<float> rotation;
    switch(orientation){
        case 0:
            rotation = rotation.Identity();
            break;
        case 1:
            rotation.SetRotationZ(0.5 * M_PI);
            break;
        case 2:
            rotation.SetRotationZ(M_PI);
            break;
        case 3:
            rotation.SetRotationZ(-0.5 * M_PI);
            break;
    }
    orientationAdjustedPerspective = perspective * rotation;
    glViewport(0, 0, width, height);
}

string View::jstringToString(JNIEnv *env, jstring jStr) {
    if (jStr == nullptr) {
        return ""; // Handle null jstring gracefully
    }

    const char *chars = env->GetStringUTFChars(jStr, nullptr);
    string result(chars);
    env->ReleaseStringUTFChars(jStr, chars);

    return result;
}

string View::stringArrayToString(string* const strings, const int &count) {
    string result = "";
    for(int i = 0; i < count; i++){
        result += strings[i];
    }
    return result;
}

// returns true if a GL error occurred
bool View::checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint View::createShader(const GLenum& shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    // Check if there were some issues when compiling the shader.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                string typeString;
                switch (shaderType) {
                    case GL_VERTEX_SHADER:
                        typeString = "vertex";
                        break;
                    case GL_TESS_CONTROL_SHADER:
                        typeString = "tesselation control";
                        break;
                    case GL_TESS_EVALUATION_SHADER:
                        typeString = "tesselation evaluation";
                        break;
                    case GL_GEOMETRY_SHADER:
                        typeString = "geometry";
                        break;
                    case GL_FRAGMENT_SHADER:
                        typeString = "fragment";
                        break;
                    case GL_COMPUTE_SHADER:
                        typeString = "compute";
                        break;
                }
                ALOGE("Could not compile %s shader:\n%s\n", typeString.c_str(), infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint checkStatus(const GLuint& program, const GLenum& statusType) {
    GLint successfulStatus = GL_FALSE;
    // Check if there were some issues when linking the shader.
    glGetProgramiv(program, statusType, &successfulStatus);
    if (!successfulStatus) {
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                ALOGE("Error linking program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

GLuint View::createComputeShaderProgram(const char* compSrc) {
    // Create the compute shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        return 0;
    }

    // Create and compile the compute shader
    GLuint shader = createShader(GL_COMPUTE_SHADER, compSrc);
    if (!shader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        return 0;
    }

    // Attach and link the shader against to the compute program
    glAttachShader(program, shader);
    glLinkProgram(program);
    glDeleteShader(shader);
    return checkStatus(program, GL_LINK_STATUS);
}

GLuint View::createVertexAndFragmentShaderProgram(const char* const vtxSrc, const char* const fragSrc) {
    // Create the shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        return 0;
    }

    // Create and compile the vertex shader
    GLuint vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        return 0;
    }

    // Create and compile the fragment shader
    GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        return 0;
    }

    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return checkStatus(program, GL_LINK_STATUS);
}

GLuint View::createVertexGeometryAndFragmentShaderProgram(const char *const vtxSrc, const char *const geomSrc, const char *const fragSrc) {
    // Create the shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        return 0;
    }

    // Create and compile the vertex shader
    GLuint vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        return 0;
    }

    // Create and compile the geometry shader
    GLuint geomShader = createShader(GL_GEOMETRY_SHADER, geomSrc);
    if (!geomShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        return 0;
    }

    // Create and compile the fragment shader
    GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(geomShader);
        return 0;
    }

    glAttachShader(program, vtxShader);
    glAttachShader(program, geomShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vtxShader);
    glDeleteShader(geomShader);
    glDeleteShader(fragShader);
    return checkStatus(program, GL_LINK_STATUS);
}

GLuint View::createVertexTesselationControlTesselationEvaluationAndFragmentShaderProgram(const char *const vtxSrc, const char *const tescSrc, const char *const teseSrc, const char *const fragSrc) {
    // Create the shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        return 0;
    }

    // Create and compile the vertex shader
    GLuint vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        return 0;
    }

    // Create and compile the tesselation control shader
    GLuint tescShader = createShader(GL_TESS_CONTROL_SHADER, tescSrc);
    if (!tescShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        return 0;
    }

    // Create and compile the tesselation evaluation shader
    GLuint teseShader = createShader(GL_TESS_EVALUATION_SHADER, teseSrc);
    if (!teseShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(tescShader);
        return 0;
    }

    // Create and compile the fragment shader
    GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(tescShader);
        glDeleteShader(teseShader);
        return 0;
    }

    glAttachShader(program, vtxShader);
    glAttachShader(program, tescShader);
    glAttachShader(program, teseShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vtxShader);
    glDeleteShader(tescShader);
    glDeleteShader(teseShader);
    glDeleteShader(fragShader);
    return checkStatus(program, GL_LINK_STATUS);
}

GLuint View::createVertexTesselationControlTesselationEvaluationGeometryAndFragmentShaderProgram(const char *const vtxSrc, const char *const tescSrc, const char *const teseSrc, const char *const geomSrc, const char *const fragSrc) {
    // Create the shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        return 0;
    }

    // Create and compile the vertex shader
    GLuint vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        return 0;
    }

    // Create and compile the tesselation control shader
    GLuint tescShader = createShader(GL_TESS_CONTROL_SHADER, tescSrc);
    if (!tescShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        return 0;
    }

    // Create and compile the tesselation evaluation shader
    GLuint teseShader = createShader(GL_TESS_EVALUATION_SHADER, teseSrc);
    if (!teseShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(tescShader);
        return 0;
    }

    // Create and compile the geometry shader
    GLuint geomShader = createShader(GL_GEOMETRY_SHADER, geomSrc);
    if (!geomShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(tescShader);
        glDeleteShader(teseShader);
        return 0;
    }

    // Create and compile the fragment shader
    GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) {
        checkGlError("createShader");
        glDeleteProgram(program);
        glDeleteShader(vtxShader);
        glDeleteShader(tescShader);
        glDeleteShader(teseShader);
        glDeleteShader(geomShader);
        return 0;
    }

    glAttachShader(program, vtxShader);
    glAttachShader(program, tescShader);
    glAttachShader(program, teseShader);
    glAttachShader(program, geomShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vtxShader);
    glDeleteShader(tescShader);
    glDeleteShader(teseShader);
    glDeleteShader(geomShader);
    glDeleteShader(fragShader);
    return checkStatus(program, GL_LINK_STATUS);
}

const bool View::supportsES32(){
    const GLenum& shaderType = GL_VERTEX_SHADER;
    const char* src =
            "#version 320 es\n"
            "void main(){\n"
            "}\n";
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        View::checkGlError("glCreateShader");
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
                    ALOGW("Could not compileFromSingleFile OpenGL ES shader version %s: %s. Compiling version 310 instead.",
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

void View::printGlString(const char* const name, const GLenum& s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

void View::incrementFrameCount() {
    framesRendered++;
}

int View::getFrameCount() {
    return framesRendered;
}
