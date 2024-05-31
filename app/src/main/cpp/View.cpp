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
        "    // Normalize the direction vector\n"
        "    vec3 normalizedDirection = normalize(direction);\n"
        "    // Calculate polar angle (theta) and azimuthal angle (phi)\n"
        "    float theta = acos(normalizedDirection.z); // polar angle\n"
        "    float phi = atan(normalizedDirection.y, normalizedDirection.x); // azimuthal angle\n"
        "    // Map angles to UV coordinates\n"
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
        "    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);\n"
        "    if (k < 0.0)\n"
        "        return reflect2(I, N, dotNI);\n"
        "    else\n"
        "        return eta * I - (eta * dotNI + sqrt(k)) * N;\n"
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
        "const float minReflectance = 0.06f; \n"
        "const float multiplier = 1.0f - pow(minReflectance, 1.0 / 3.0); \n"
        "float fresnel(float dotNI){ \n"
        //"    return pow(1.0 + dotNI, 3.0); \n"
        //"    float F0 = 0.4f; \n"
        //"    float f = pow(1.0 - dotNI, 5.0); \n"
        //"    return F0 + (1.0 - F0) * f; \n"
        "    return pow(1.0 + multiplier * dotNI, 3.0); \n" // Reflectance in [minReflectance, 1.0]
        "} \n";

int View:: backgroundTexture = 0;

View::View() : framesRendered(0), distanceToOrigin(0), mEglContext(eglGetCurrentContext()){

}

View::~View(){
    if (eglGetCurrentContext() != mEglContext) return;
}

vec3 View::compensateForOrientation(const vec3& acc){
    switch(orientation){
        case 0:
            return -1.0f * acc;
            break;
        case 1:
            return vec3(acc.y, -acc.x, -acc.z);
            break;
        case 2:
            return -1.0f * acc;
            break;
        case 3:
            return vec3(-acc.y, acc.x, -acc.z);
            break;
    }
}

void View::render(){

}

void View::calculatePerspectiveSetViewport(const float& maxViewDegrees, const float& znear, const float& zfar) {
    float aspect = (float)width / (float)height;
    float verticalScreenAngle = aspect < 1.0f ? toRadians(maxViewDegrees) : 2.0f * atanf(tanf(0.5f * toRadians(maxViewDegrees)) / aspect);
    perspective.SetPerspective(verticalScreenAngle, aspect, znear, zfar);
    Matrix4<float> rotation;
    switch(orientation){
        case 0:
            rotation.SetRotationZ(0.0 * M_PI); // Up
            break;
        case 1:
            rotation.SetRotationZ(0.5 * M_PI); // Left
            break;
        case 2:
            rotation.SetRotationZ(1.0 * M_PI); // Down
            break;
        case 3:
            rotation.SetRotationZ(-0.5 * M_PI); // Right
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
    glCompileShader(shader);
    GLint compiled = GL_FALSE;
    // Check if there were some issues when compiling the shader.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(sizeof(GLchar) * infoLogLen);
            if (infoLog) {
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
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                ALOGE("Could not compile %s shader:\n%s\n", typeString.c_str(), infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint View::buildShaderProgram(const GLenum* const shaderTypes, const char** const sources, const int& shaderCount) {
    vector<GLuint> shaders;
    for(int i = 0; i < shaderCount; i++) {
        GLuint currentShader = createShader(shaderTypes[i], sources[i]);
        if (!currentShader) {
            for (const GLuint& shader : shaders) {
                glDeleteShader(shader);
            }
            return 0;
        }
        shaders.push_back(currentShader);
    }

    // Create the compute shader program
    GLuint program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        for (const GLuint& shader : shaders) {
            glDeleteShader(shader);
        }
        return 0;
    }

    // Attach and link the compute shader against the compute program
    for (const GLuint& shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    for (const GLuint& shader : shaders) {
        glDeleteShader(shader);
    }

    GLint linked = GL_FALSE;
    // Check if there were some issues when linking the shader.
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 1) {
            GLchar* infoLog = (GLchar*)malloc(sizeof(GLchar) * infoLogLen);
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
    const GLenum shaderTypes[1] = {
            GL_COMPUTE_SHADER
    };
    const char* sources[1] = {
            compSrc
    };
    return buildShaderProgram(shaderTypes, sources, 1);
}

GLuint View::createVertexAndFragmentShaderProgram(const char* const vertexShaderSource, const char* const fragmentShaderSource) {
    const GLenum shaderTypes[2] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER
    };
    const char* sources[2] = {
            vertexShaderSource,
            fragmentShaderSource
    };
    return buildShaderProgram(shaderTypes, sources, 2);
}

GLuint View::createVertexGeometryAndFragmentShaderProgram(const char *const vertexShaderSource, const char *const geometryShaderSource, const char *const fragmentShaderSource) {
    const GLenum shaderTypes[3] = {
            GL_VERTEX_SHADER,
            GL_GEOMETRY_SHADER,
            GL_FRAGMENT_SHADER
    };
    const char* sources[3] = {
            vertexShaderSource,
            geometryShaderSource,
            fragmentShaderSource
    };
    return buildShaderProgram(shaderTypes, sources, 3);
}

GLuint View::createVertexTesselationControlTesselationEvaluationAndFragmentShaderProgram(const char *const vertexShaderSource, const char *const tesselationControlShaderSource, const char *const tesselationEvaluationShaderSource, const char *const fragmentShaderSource) {
    const GLenum shaderTypes[4] = {
            GL_VERTEX_SHADER,
            GL_TESS_CONTROL_SHADER,
            GL_TESS_EVALUATION_SHADER,
            GL_FRAGMENT_SHADER
    };
    const char* sources[4] = {
            vertexShaderSource,
            tesselationControlShaderSource,
            tesselationEvaluationShaderSource,
            fragmentShaderSource
    };
    return buildShaderProgram(shaderTypes, sources, 4);
}

GLuint View::createVertexTesselationControlTesselationEvaluationGeometryAndFragmentShaderProgram(const char *const vertexShaderSource, const char *const tesselationControlShaderSource, const char *const tesselationEvaluationShaderSource, const char *const geometryShaderSource, const char *const fragmentShaderSource) {
    const GLenum shaderTypes[5] = {
            GL_VERTEX_SHADER,
            GL_TESS_CONTROL_SHADER,
            GL_TESS_EVALUATION_SHADER,
            GL_GEOMETRY_SHADER,
            GL_FRAGMENT_SHADER
    };
    const char* sources[5] = {
            vertexShaderSource,
            tesselationControlShaderSource,
            tesselationEvaluationShaderSource,
            geometryShaderSource,
            fragmentShaderSource
    };
    return buildShaderProgram(shaderTypes, sources, 5);
}

const bool View::supportsES32(){
    const GLenum& shaderTypes = GL_VERTEX_SHADER;
    const char* src =
            "#version 320 es\n"
            "void main(){\n"
            "}\n";
    GLuint shader = glCreateShader(shaderTypes);
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

vec3 View::computeForce(const float& gravity, const bool& referenceFrameRotates, const Matrix4<float> rotation) {
    float linearAccelerationMultiplier = 8.0f * (getFrameCount() > 10 ? 1.0f : 1.0f / 10 * getFrameCount());
    if(referenceFrameRotates){
        Vec3<float> force = (rotation * Vec4<float>(0.0f, 0.0f, 9.81f, 1.0f)).XYZ();
        return compensateForOrientation(linearAccelerationMultiplier * linearAccelerationVector + gravity * vec3(force.x, force.y, force.z));
    }else {
        return quaternionTo3x3(rotationVector) * (-linearAccelerationMultiplier * linearAccelerationVector) + gravity * vec3(0.0f, 0.0f, -9.81f);
    }
}
