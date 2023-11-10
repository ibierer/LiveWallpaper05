#include "gles3jni.h"

#include <jni.h>
#include <string>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stack>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_livewallpaper05_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

const Vertex QUAD[4] = {
        // Square with diagonal < 2 so that it fits in a [-1 .. 1]^2 square
        // regardless of rotation.
        {{-0.7f, -0.7f, -1.0f}, {0x00, 0xFF, 0x00}},
        {{0.7f, -0.7f, 1.0f}, {0x00, 0x00, 0xFF}},
        {{-0.7f, 0.7f, -1.0f}, {0xFF, 0x00, 0x00}},
        {{0.7f, 0.7f, 1.0f}, {0xFF, 0xFF, 0xFF}},
};

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

// ----------------------------------------------------------------------------

Renderer::Renderer() : mLastFrameNs(0) {
    memset(mScale, 0, sizeof(mScale));
    memset(&mAngularVelocity, 0, sizeof(float));
    memset(&mAngles, 0, sizeof(float));
}

Renderer::~Renderer() {}

void Renderer::resize(int w, int h) {
    auto offsets = mapOffsetBuf();
    calcSceneParams(w, h, offsets);
    unmapOffsetBuf();

    // Auto gives a signed int :-(
    mAngles = drand48() * TWO_PI;
    mAngularVelocity = MAX_ROT_SPEED * (2.0 * drand48() - 1.0);

    mLastFrameNs = 0;

    glViewport(0, 0, w, h);
}

void Renderer::calcSceneParams(unsigned int w, unsigned int h, float* offsets) {
    // Calculations are done in "landscape", i.e. assuming dim[0] >= dim[1].
    // Only at the end are values put in the opposite order if h > w.
    const float dim[2] = {fmaxf(w, h), fminf(w, h)};
    const float aspect[2] = {dim[0] / dim[1], dim[1] / dim[0]};
    const float scene2clip[2] = {1.0f, aspect[0]};
    const int ncells[2] = {static_cast<int>(1),
                           (int)floorf(aspect[1])};

    float centers[2][MAX_INSTANCES_PER_SIDE];
    for (int d = 0; d < 2; d++) {
        auto offset = -ncells[d] / 1;  // -1.0 for d=0
        for (auto i = 0; i < ncells[d]; i++) {
            centers[d][i] = scene2clip[d] * (2.0f * (i + 0.5f) + offset);
        }
    }

    int major = w >= h ? 0 : 1;
    int minor = w >= h ? 1 : 0;
    offsets[major] = centers[0][0];
    offsets[minor] = centers[1][0];

    mScale[major] = 0.5f * 2.0f * scene2clip[0];
    mScale[minor] = 0.5f * 2.0f * scene2clip[1];
}

void Renderer::step() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    auto nowNs = now.tv_sec * 1000000000ull + now.tv_nsec;

    if (mLastFrameNs > 0) {
        float dt = float(nowNs - mLastFrameNs) * 0.000000001f;

        mAngles += mAngularVelocity * dt;
        if (mAngles >= TWO_PI) {
            mAngles -= TWO_PI;
        } else if (mAngles <= -TWO_PI) {
            mAngles += TWO_PI;
        }

        float* transforms = mapTransformBuf();
        float s = sinf(mAngles);
        float c = cosf(mAngles);
        transforms[0] = c * mScale[0];
        transforms[1] = s * mScale[1];
        transforms[2] = -s * mScale[0];
        transforms[3] = c * mScale[1];
        unmapTransformBuf();
    }

    mLastFrameNs = nowNs;
}

void Renderer::render() {
    step();

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(1);
    checkGlError("Renderer::render");
}

// ----------------------------------------------------------------------------

static Renderer* g_renderer = NULL;

extern "C" {
JNIEXPORT void JNICALL Java_com_example_livewallpaper05_MainActivity_00024Companion_init(JNIEnv* env,
                                                                  jobject obj);
JNIEXPORT void JNICALL Java_com_example_livewallpaper05_MainActivity_00024Companion_resize(
        JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_example_livewallpaper05_MainActivity_00024Companion_step(JNIEnv* env,
                                                                  jobject obj, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w);
};

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() { return GL_TRUE; }
#endif

//#define TRY_ES_UTIL
#ifdef TRY_ES_UTIL
int screen_width;
int screen_height;
bool initiated = false;
void onDrawFrame(float acc_x, float acc_y, float acc_z, float rot_x, float rot_y, float rot_z, float rot_w);
void onSurfaceChanged(int width, int height);
void onSurfaceCreated();
#endif

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_init(JNIEnv *env, jobject thiz) {
#ifndef TRY_ES_UTIL
    if (g_renderer) {
        delete g_renderer;
        g_renderer = NULL;
    }

    printGlString("Version", GL_VERSION);
    printGlString("Vendor", GL_VENDOR);
    printGlString("Renderer", GL_RENDERER);
    printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        g_renderer = createES3Renderer();
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        //g_renderer = createES2Renderer();
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
#else
    onSurfaceCreated();
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_resize(JNIEnv *env, jobject thiz,
                                                                    jint width, jint height) {
#ifndef TRY_ES_UTIL
    if (g_renderer) {
        g_renderer->resize(width, height);
    }
#else
    onSurfaceChanged(width, height);
#endif
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w) {
#ifndef TRY_ES_UTIL
    if (g_renderer) {
        g_renderer->render();
    }
#else
    onDrawFrame(acc_x, acc_y, acc_z, rot_x, rot_y, rot_z, rot_w);
#endif
}




































#ifdef TRY_ES_UTIL

template <class T>
class _vec2 {
public:
    union {
        T v[2];
        struct {
            T x;
            T y;
        };
        struct {
            T r;
            T g;
        };
    };
    T& operator[](int index) {
        return v[index];
    }
    _vec2() {

    }
    _vec2(const T input) {
        for (int i = 0; i < 2; i++) this->v[i] = input;
    }
    _vec2(const T x, const T y) {
        this->x = x; this->y = y;
    }
    _vec2(const T array[2]) {
        for (int i = 0; i < 2; i++) this->v[i] = array[i];
    }
    _vec2(const _vec2<unsigned int>& input) {
        for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<int>& input) {
        for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<float>& input) {
        for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
    }
    _vec2(const _vec2<double>& input) {
        for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
    }
    _vec2 operator+(_vec2 input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = this->v[i] + input[i];
        return _return;
    }
    _vec2& operator+=(_vec2 input) {
        for (int i = 0; i < 2; i++) v[i] += input[i];
        return *this;
    }
    _vec2 operator-(_vec2 input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] - input[i];
        return _return;
    }
    _vec2& operator-=(_vec2 input) {
        for (int i = 0; i < 2; i++) v[i] -= input[i];
        return *this;
    }
    _vec2 operator-() {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = -v[i];
        return _return;
    }
    _vec2 operator*(_vec2 input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * input[i];
        return _return;
    }
    _vec2& operator*=(_vec2 input) {
        for (int i = 0; i < 2; i++) v[i] *= input[i];
        return *this;
    }
    _vec2 operator*(T scalar) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec2& operator*=(T scalar) {
        for (int i = 0; i < 2; i++) v[i] *= scalar;
        return *this;
    }
    _vec2 operator/(_vec2 input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input[i];
        return _return;
    }
    _vec2& operator/=(_vec2 input) {
        for (int i = 0; i < 2; i++) v[i] /= input[i];
        return *this;
    }
    _vec2 operator/(T input) {
        _vec2 _return;
        for (int i = 0; i < 2; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec2& operator/=(T input) {
        for (int i = 0; i < 2; i++) v[i] /= input;
        return *this;
    }
    _vec2& operator=(_vec2<unsigned int> rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(_vec2<int> rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec2& operator=(_vec2<float> rhs) {
        for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(_vec2 rhs) {
        return rhs.x == x && rhs.y == y;
    }
    bool operator!=(_vec2 rhs) {
        return rhs.x != x || rhs.y != y;
    }
    //std::string str(){
    //	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    //}
};

//template <typename T>
//std::string std::to_string(_vec2<T> input) {
//    return input.str();
//}

template <typename T>
_vec2<T> operator*(T lhs, _vec2<T> rhs) {
    _vec2<T> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec2<float> operator*(float lhs, _vec2<int> rhs) {
    _vec2<float> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec2<float> operator*(_vec2<int> lhs, float rhs) {
    return rhs * lhs;
}
_vec2<float> operator*(int lhs, _vec2<float> rhs) {
    _vec2<float> _return;
    for (int i = 0; i < 2; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec2<float> operator*(_vec2<float> lhs, int rhs) {
    return rhs * lhs;
}

_vec2<double> operator*(double lhs, _vec2<float> rhs) {
    _vec2<double> _return;
    for (int i = 0; i < 2; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec2<double> operator*(_vec2<float> lhs, double rhs) {
    return rhs * lhs;
}
_vec2<double> operator*(float lhs, _vec2<double> rhs) {
    _vec2<double> _return;
    for (int i = 0; i < 2; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec2<double> operator*(_vec2<double> lhs, float rhs) {
    return rhs * lhs;
}

template <class T>
class _vec3 {
public:
    union {
        T v[3];
        _vec2<T> xy;
        struct {
            T x;
            T y;
            T z;
        };
        struct {
            T r;
            T g;
            T b;
        };
    };
    T& operator[](int index) {
        return v[index];
    }
    _vec3() {

    }
    _vec3(const T input) {
        for (int i = 0; i < 3; i++) this->v[i] = input;
    }
    _vec3(const T x, const T y, const T z) {
        this->x = x; this->y = y; this->z = z;
    }
    _vec3(const T array[3]) {
        for (int i = 0; i < 3; i++) this->v[i] = array[i];
    }
    _vec3(const _vec2<T>& input, const T& z) {//Should try reducing to just one loop of 3 iterations
        for (int i = 0; i < 2; i++) this->v[i] = input[i]; this->v[2] = z;
    }
    _vec3(const _vec3<unsigned int>& input) {
        for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<int>& input) {
        for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<float>& input) {
        for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
    }
    _vec3(const _vec3<double>& input) {
        for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
    }
    _vec3 operator+(_vec3 input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] + input[i];
        return _return;
    }
    _vec3& operator+=(_vec3 input) {
        for (int i = 0; i < 3; i++) v[i] += input[i];
        return *this;
    }
    _vec3 operator-(_vec3 input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] - input[i];
        return _return;
    }
    _vec3& operator-=(_vec3 input) {
        for (int i = 0; i < 3; i++) v[i] -= input[i];
        return *this;
    }
    _vec3 operator-() {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = -v[i];
        return _return;
    }
    _vec3 operator*(_vec3 input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * input[i];
        return _return;
    }
    _vec3& operator*=(_vec3 input) {
        for (int i = 0; i < 3; i++) v[i] *= input[i];
        return *this;
    }
    _vec3 operator*(T scalar) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec3& operator*=(T scalar) {
        for (int i = 0; i < 3; i++) v[i] *= scalar;
        return *this;
    }
    _vec3 operator/(_vec3 input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input[i];
        return _return;
    }
    _vec3& operator/=(_vec3 input) {
        for (int i = 0; i < 3; i++) v[i] /= input[i];
        return *this;
    }
    _vec3 operator/(T input) {
        _vec3 _return;
        for (int i = 0; i < 3; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec3& operator/=(T input) {
        for (int i = 0; i < 3; i++) v[i] /= input;
        return *this;
    }
    _vec3& operator=(_vec3<unsigned int> rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(_vec3<int> rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec3& operator=(_vec3<float> rhs) {
        for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(_vec3 rhs) {
        return rhs.x == x && rhs.y == y && rhs.z == z;
    }
    bool operator!=(_vec3 rhs) {
        return rhs.x != x || rhs.y != y || rhs.z != z;
    }
    //std::string str(){
    //	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    //}
};

//template <typename T>
//std::string std::to_string(_vec3<T> input) {
//    return input.str();
//}

template <typename T>
_vec3<T> operator*(T lhs, _vec3<T> rhs) {
    _vec3<T> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec3<float> operator*(float lhs, _vec3<int> rhs) {
    _vec3<float> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec3<float> operator*(_vec3<int> lhs, float rhs) {
    return rhs * lhs;
}
_vec3<float> operator*(int lhs, _vec3<float> rhs) {
    _vec3<float> _return;
    for (int i = 0; i < 3; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec3<float> operator*(_vec3<float> lhs, int rhs) {
    return rhs * lhs;
}

_vec3<double> operator*(double lhs, _vec3<float> rhs) {
    _vec3<double> _return;
    for (int i = 0; i < 3; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec3<double> operator*(_vec3<float> lhs, double rhs) {
    return rhs * lhs;
}
_vec3<double> operator*(float lhs, _vec3<double> rhs) {
    _vec3<double> _return;
    for (int i = 0; i < 3; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec3<double> operator*(_vec3<double> lhs, float rhs) {
    return rhs * lhs;
}

template <class T>
class _vec4 {
public:
    union {
        T v[4];
        _vec2<T> xy;
        _vec3<T> xyz;
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        struct {
            T r;
            T g;
            T b;
            T a;
        };
    };
    T& operator[](int index) {
        return v[index];
    }
    _vec4() {

    }
    _vec4(const T input) {
        for (int i = 0; i < 4; i++) this->v[i] = input;
    }
    _vec4(const T x, const T y, const T z, const T w) {
        this->x = x; this->y = y; this->z = z; this->w = w;
    }
    _vec4(const T array[4]) {
        for (int i = 0; i < 4; i++) this->v[i] = array[i];
    }
    _vec4(const _vec3<T>& input, const T& z) {//Should try reducing to just one loop of 4 iterations
        for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i]; this->v[3] = (T)z;
    }
    _vec4(const _vec4<unsigned int>& input) {
        for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<int>& input) {
        for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<float>& input) {
        for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
    }
    _vec4(const _vec4<double>& input) {
        for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
    }
    _vec4 operator+(_vec4 input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] + input[i];
        return _return;
    }
    _vec4& operator+=(_vec4 input) {
        for (int i = 0; i < 4; i++) v[i] += input[i];
        return *this;
    }
    _vec4 operator-(_vec4 input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] - input[i];
        return _return;
    }
    _vec4& operator-=(_vec4 input) {
        for (int i = 0; i < 4; i++) v[i] -= input[i];
        return *this;
    }
    _vec4 operator-() {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = -v[i];
        return _return;
    }
    _vec4 operator*(_vec4 input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * input[i];
        return _return;
    }
    _vec4& operator*=(_vec4 input) {
        for (int i = 0; i < 4; i++) v[i] *= input[i];
        return *this;
    }
    _vec4 operator*(T scalar) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] * scalar;
        return _return;
    }
    _vec4& operator*=(T scalar) {
        for (int i = 0; i < 4; i++) v[i] *= scalar;
        return *this;
    }
    _vec4 operator/(_vec4 input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input[i];
        return _return;
    }
    _vec4& operator/=(_vec4 input) {
        for (int i = 0; i < 4; i++) v[i] /= input[i];
        return *this;
    }
    _vec4 operator/(T input) {
        _vec4 _return;
        for (int i = 0; i < 4; i++) _return[i] = v[i] / input;
        return _return;
    }
    _vec4& operator/=(T input) {
        for (int i = 0; i < 4; i++) v[i] /= input;
        return *this;
    }
    _vec4& operator=(_vec4<unsigned int> rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(_vec4<int> rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    _vec4& operator=(_vec4<float> rhs) {
        for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
        return *this;
    }
    bool operator==(_vec4 rhs) {
        return rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w;
    }
    bool operator!=(_vec4 rhs) {
        return rhs.x != x || rhs.y != y || rhs.z != z || rhs.w != w;
    }
    //std::string str(){
    //	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
    //}
};

//template <typename T>
//std::string std::to_string(_vec4<T> input) {
//    return input.str();
//}

template <typename T>
_vec4<T> operator*(T lhs, _vec4<T> rhs) {
    _vec4<T> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (T)rhs.v[i];
    return _return;
}

_vec4<float> operator*(float lhs, _vec4<int> rhs) {
    _vec4<float> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (float)rhs.v[i];
    return _return;
}
_vec4<float> operator*(_vec4<int> lhs, float rhs) {
    return rhs * lhs;
}
_vec4<float> operator*(int lhs, _vec4<float> rhs) {
    _vec4<float> _return;
    for (int i = 0; i < 4; i++) _return[i] = (float)lhs * rhs.v[i];
    return _return;
}
_vec4<float> operator*(_vec4<float> lhs, int rhs) {
    return rhs * lhs;
}

_vec4<double> operator*(double lhs, _vec4<float> rhs) {
    _vec4<double> _return;
    for (int i = 0; i < 4; i++) _return[i] = lhs * (double)rhs.v[i];
    return _return;
}
_vec4<double> operator*(_vec4<float> lhs, double rhs) {
    return rhs * lhs;
}
_vec4<double> operator*(float lhs, _vec4<double> rhs) {
    _vec4<double> _return;
    for (int i = 0; i < 4; i++) _return[i] = (double)lhs * rhs.v[i];
    return _return;
}
_vec4<double> operator*(_vec4<double> lhs, float rhs) {
    return rhs * lhs;
}

typedef _vec2<float> vec2;
typedef _vec2<unsigned int> uvec2;
typedef _vec2<int> ivec2;
typedef _vec2<unsigned char> ucvec2;

typedef _vec3<float> vec3;
typedef _vec3<unsigned int> uvec3;
typedef _vec3<int> ivec3;
typedef _vec3<unsigned char> ucvec3;

typedef _vec4<float> vec4;
typedef _vec4<unsigned int> uvec4;
typedef _vec4<int> ivec4;
typedef _vec4<unsigned char> ucvec4;

vec3 cross(vec3 left, vec3 right) {
    return vec3(
            left.y * right.z - left.z * right.y,
            -left.x * right.z + left.z * right.x,
            left.x * right.y - left.y * right.x
    );
}
float dot(vec2 left, vec2 right) {
    vec2 product = left * right;
    return product.x + product.y;
}
float dot(vec3 left, vec3 right) {
    vec3 product = left * right;
    return product.x + product.y + product.z;
}
float dot(vec4 left, vec4 right) {
    vec4 product = left * right;
    return product.x + product.y + product.z + product.w;
}
float distance(vec2 coordinates) {
    return sqrt(dot(coordinates, coordinates));
}
float distance(vec3 coordinates) {
    return sqrt(dot(coordinates, coordinates));
}
float distance(vec2 coordinates1, vec2 coordinates2) {
    return distance(coordinates2 - coordinates1);
}
float distance(vec3 coordinates1, vec3 coordinates2) {
    return distance(coordinates2 - coordinates1);
}
float length(vec2 coordinates) {
    return distance(coordinates);
}
float length(vec3 coordinates) {
    return distance(coordinates);
}
template <class T>
void clamp(int vectorDimensionCount, T* x, T* minVal, T* maxVal) {
    for (int i = 0; i < vectorDimensionCount; i++) {
        if (x[i] < minVal[i])
            x[i] = minVal[i];
        if (x[i] > maxVal[i])
            x[i] = maxVal[i];
    }
}
float clamp(float x, float minVal, float maxVal) {
    clamp<float>(1, (float*)&x, (float*)&minVal, (float*)&maxVal);
    return x;
}
vec2 clamp(vec2 x, vec2 minVal, vec2 maxVal) {
    clamp<float>(2, (float*)&x, (float*)&minVal, (float*)&maxVal);
    return x;
}
vec3 clamp(vec3 x, vec3 minVal, vec3 maxVal) {
    clamp<float>(3, (float*)&x, (float*)&minVal, (float*)&maxVal);
    return x;
}
vec4 clamp(vec4 x, vec4 minVal, vec4 maxVal) {
    clamp<float>(4, (float*)&x, (float*)&minVal, (float*)&maxVal);
    return x;
}


float distance(const float x, const float y) {
    return sqrt(x * x + y * y);
}
float distance(const float x, const float y, const float z) {
    return sqrt(x * x + y * y + z * z);
}
float toDegrees(const float radians) {
    return radians * 180.0f / M_PI;
}
float toRadians(const float degrees) {
    return degrees * M_PI / 180.0f;
}
vec2 normalize(vec2 input) {
    return input / distance(input);
}
vec3 normalize(vec3 input) {
    return input / distance(input);
}

class mat3 {
public:
    vec3 m[3];
    mat3() {

    }
    mat3(vec3 a, vec3 b, vec3 c) {
        m[0] = a;
        m[1] = b;
        m[2] = c;
    }
    vec3 operator*(vec3 input) {
        vec3 _return;
        for (int i = 0; i < 3; i++) {
            _return[i] = dot(m[i], input);
        }
        return _return;
    }
};

mat3 quaternionTo3x3(vec4 input) {
    float thetaOver2 = acosf(input[3]);
    float sinOfHalfTheta = sinf(thetaOver2);
    float x = input[0] / sinOfHalfTheta;
    float y = input[1] / sinOfHalfTheta;
    float z = input[2] / sinOfHalfTheta;
    float theta = 2.0f * thetaOver2;
    return mat3(
            vec3(x * x * (1 - cosf(theta)) + cosf(theta), x * y * (1 - cosf(theta)) - z * sinf(theta), x * z * (1 - cosf(theta)) + y * sinf(theta)),
            vec3(x * y * (1 - cosf(theta)) + z * sinf(theta), y * y * (1 - cosf(theta)) + cosf(theta), y * z * (1 - cosf(theta)) - x * sinf(theta)),
            vec3(x * z * (1 - cosf(theta)) - y * sinf(theta), y * z * (1 - cosf(theta)) + x * sinf(theta), z * z * (1 - cosf(theta)) + cosf(theta))
    );
}



void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}
void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = (unsigned char*)ptr;
    unsigned char v = (unsigned char)value;

    while (num--) {
        *p++ = v;
    }

    return ptr;
}

#define ESUTIL_API
#define ESCALLBACK

typedef struct {
    GLfloat m[4][4];
} ESMatrix;

typedef struct ESContext ESContext;
#define ESUTIL_API
#define ESCALLBACK

struct ESContext {
    /// Put platform specific data here
    //void* platformData;

    /// Put your user data here...
    //* userData;

    /// Window width
    //GLint       width;

    /// Window height
    //GLint       height;

#ifndef __APPLE__
    /// Display handle
    //EGLNativeDisplayType eglNativeDisplay;

    /// Window handle
    //EGLNativeWindowType  eglNativeWindow;

    /// EGL display
    //EGLDisplay  eglDisplay;

    /// EGL context
    //EGLContext  eglContext;

    /// EGL surface
    //EGLSurface  eglSurface;
#endif

    /// Callbacks
    void (ESCALLBACK* drawFunc) (ESContext*);
    void (ESCALLBACK* shutdownFunc) (ESContext*);
    void (ESCALLBACK* keyFunc) (ESContext*, unsigned char, int, int);
    void (ESCALLBACK* updateFunc) (ESContext*, float deltaTime);
};

GLboolean ESUTIL_API esCreateWindow(ESContext* esContext, const char* title, GLint width, GLint height, GLuint flags);
void ESUTIL_API esRegisterDrawFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*));
void ESUTIL_API esRegisterShutdownFunc(ESContext* esContext, void (ESCALLBACK* shutdownFunc) (ESContext*));
void ESUTIL_API esRegisterUpdateFunc(ESContext* esContext, void (ESCALLBACK* updateFunc) (ESContext*, float));
void ESUTIL_API esRegisterKeyFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*, unsigned char, int, int));
void ESUTIL_API esLogMessage(const char* formatStr, ...);
GLuint ESUTIL_API esLoadShader(GLenum type, const char* shaderSrc);
GLuint ESUTIL_API esLoadProgram(const char* vertShaderSrc, const char* fragShaderSrc);
int ESUTIL_API esGenSphere(int numSlices, float radius, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices);
int ESUTIL_API esGenCube(float scale, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices);
int ESUTIL_API esGenSquareGrid(int size, GLfloat** vertices, GLuint** indices);
char* ESUTIL_API esLoadTGA(void* ioContext, const char* fileName, int* width, int* height);
void ESUTIL_API esScale(ESMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz);
void ESUTIL_API esTranslate(ESMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz);
void ESUTIL_API esRotate(ESMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void ESUTIL_API esFrustum(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ);
void ESUTIL_API esPerspective(ESMatrix* result, float fovy, float aspect, float nearZ, float farZ);
void ESUTIL_API esOrtho(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ);
void ESUTIL_API esMatrixMultiply(ESMatrix* result, ESMatrix* srcA, ESMatrix* srcB);
void ESUTIL_API esMatrixLoadIdentity(ESMatrix* result);
void ESUTIL_API esMatrixLookAt(ESMatrix* result, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
///
//  Public Functions
//

//
/// \brief Create a window with the specified parameters
/// \param esContext Application context
/// \param title Name for title bar of window
/// \param width Width in pixels of window to create
/// \param height Height in pixels of window to create
/// \param flags Bitfield for the window creation flags
///         ES_WINDOW_RGB     - specifies that the color buffer should have R,G,B channels
///         ES_WINDOW_ALPHA   - specifies that the color buffer should have alpha
///         ES_WINDOW_DEPTH   - specifies that a depth buffer should be created
///         ES_WINDOW_STENCIL - specifies that a stencil buffer should be created
///         ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
/// \return GL_TRUE if window creation is succesful, GL_FALSE otherwise

//GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags );
//GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, LPCTSTR title, GLint width, GLint height, GLuint flags )
//{
//   EGLint configAttribList[] =
//   {
//	   EGL_RED_SIZE,       5,
//	   EGL_GREEN_SIZE,     6,
//	   EGL_BLUE_SIZE,      5,
//	   EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
//	   EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
//	   EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
//	   EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
//	   EGL_NONE
//   };
//   EGLint surfaceAttribList[] =
//   {
//	   EGL_POST_SUB_BUFFER_SUPPORTED_NV, flags & (ES_WINDOW_POST_SUB_BUFFER_SUPPORTED) ? EGL_TRUE : EGL_FALSE,
//	   EGL_NONE, EGL_NONE
//   };
//
//   if ( esContext == NULL )
//   {
//	  return GL_FALSE;
//   }
//
//   esContext->width = width;
//   esContext->height = height;
//
//   if ( !WinCreate ( esContext, title) )
//   {
//	  return GL_FALSE;
//   }
//
//
//   if ( !CreateEGLContext ( esContext->hWnd,
//							&esContext->eglDisplay,
//							&esContext->eglContext,
//							&esContext->eglSurface,
//							configAttribList,
//							surfaceAttribList ) )
//   {
//	  return GL_FALSE;
//   }
//
//
//   return GL_TRUE;
//}

//
/// \brief Register a draw callback function to be used to render each frame
/// \param esContext Application context
/// \param drawFunc Draw callback function that will be used to render the scene
//

//void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext * ) );
void ESUTIL_API esRegisterDrawFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*))
{
    esContext->drawFunc = drawFunc;
}

//
/// \brief Register a callback function to be called on shutdown
/// \param esContext Application context
/// \param shutdownFunc Shutdown callback function
//

//void ESUTIL_API esRegisterShutdownFunc ( ESContext *esContext, void ( ESCALLBACK *shutdownFunc ) ( ESContext * ) );
void ESUTIL_API esRegisterShutdownFunc(ESContext* esContext, void (ESCALLBACK* shutdownFunc) (ESContext*))
{
    esContext->shutdownFunc = shutdownFunc;
}

//
/// \brief Register an update callback function to be used to update on each time step
/// \param esContext Application context
/// \param updateFunc Update callback function that will be used to render the scene
//

//void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void ( ESCALLBACK *updateFunc ) ( ESContext *, float ) );
void ESUTIL_API esRegisterUpdateFunc(ESContext* esContext, void (ESCALLBACK* updateFunc) (ESContext*, float))
{
    esContext->updateFunc = updateFunc;
}

//
/// \brief Register a keyboard input processing callback function
/// \param esContext Application context
/// \param keyFunc Key callback function for application processing of keyboard input
//

//void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext *, unsigned char, int, int ) );
void ESUTIL_API esRegisterKeyFunc(ESContext* esContext, void (ESCALLBACK* keyFunc) (ESContext*, unsigned char, int, int))
{
    esContext->keyFunc = keyFunc;
}
//
/// \brief Log a message to the debug output for the platform
/// \param formatStr Format string for error log.
//

//void ESUTIL_API esLogMessage ( const char *formatStr, ... );
void ESUTIL_API esLogMessage(const char* formatStr, ...)
{
    va_list params;
    char buf[BUFSIZ];

    va_start(params, formatStr);
    //vsprintf_s ( buf, sizeof(buf),  formatStr, params );
    vsprintf(buf, formatStr, params);

    printf("%s", buf);

    va_end(params);
}

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//

//GLuint ESUTIL_API esLoadShader ( GLenum type, const char *shaderSrc );
GLuint ESUTIL_API esLoadShader(GLenum type, const char* shaderSrc) {
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);
    if (shader == 0) return 0;
    // Load the shader sourcem
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            //char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            //glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            //esLogMessage("Error compiling shader:\n%s\n", infoLog);
            //free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
///        Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//

//GLuint ESUTIL_API esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );
GLuint ESUTIL_API esLoadProgram(const char* vertShaderSrc, const char* fragShaderSrc) {
    //https://git.okoyono.de/klaute/OPiGPGPUTest/commit/529a8700b73e5b60238d38f1a1f9f51684a11218
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    // Load the vertex/fragment shaders
    vertexShader = esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);
    if (vertexShader == 0) return 0;
    fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    // Create the program object
    programObject = glCreateProgram();
    if (programObject == 0) return 0;
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    // Link the program
    glLinkProgram(programObject);
    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            //char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            //glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            //esLogMessage("Error linking program:\n%s\n", infoLog);
            //free(infoLog);
        }
        glDeleteProgram(programObject);
        return 0;
    }
    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return programObject;
}


//
/// \brief Generates geometry for a sphere.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLE_STRIP
/// \param numSlices The number of slices in the sphere
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLE_STRIP
//

//int ESUTIL_API esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint **indices );
int ESUTIL_API esGenSphere(int numSlices, float radius, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices)
{
    //int i;
    //int j;
    //int numParallels = numSlices / 2;
    //int numVertices = ( numParallels + 1 ) * ( numSlices + 1 );
    //int numIndices = numParallels * numSlices * 6;
    //float angleStep = (2.0f * ES_PI) / ((float) numSlices);

    //// Allocate memory for buffers
    //if ( vertices != NULL )
    //	*vertices = malloc ( sizeof(GLfloat) * 3 * numVertices );

    //if ( normals != NULL )
    //	*normals = malloc ( sizeof(GLfloat) * 3 * numVertices );

    //if ( texCoords != NULL )
    //	*texCoords = malloc ( sizeof(GLfloat) * 2 * numVertices );

    //if ( indices != NULL )
    //	*indices = malloc ( sizeof(GLuint) * numIndices );

    //for ( i = 0; i < numParallels + 1; i++ )
    //{
    //	for ( j = 0; j < numSlices + 1; j++ )
    //	{
    //		int vertex = ( i * (numSlices + 1) + j ) * 3;

    //		if ( vertices )
    //		{
    //			(*vertices)[vertex + 0] = radius * sinf ( angleStep * (float)i ) *
    //											   sinf ( angleStep * (float)j );
    //			(*vertices)[vertex + 1] = radius * cosf ( angleStep * (float)i );
    //			(*vertices)[vertex + 2] = radius * sinf ( angleStep * (float)i ) *
    //											   cosf ( angleStep * (float)j );
    //		}

    //		if ( normals )
    //		{
    //			(*normals)[vertex + 0] = (*vertices)[vertex + 0] / radius;
    //			(*normals)[vertex + 1] = (*vertices)[vertex + 1] / radius;
    //			(*normals)[vertex + 2] = (*vertices)[vertex + 2] / radius;
    //		}

    //		if ( texCoords )
    //		{
    //			int texIndex = ( i * (numSlices + 1) + j ) * 2;
    //			(*texCoords)[texIndex + 0] = (float) j / (float) numSlices;
    //			(*texCoords)[texIndex + 1] = ( 1.0f - (float) i ) / (float) (numParallels - 1 );
    //		}
    //	}
    //}

    //// Generate the indices
    //if ( indices != NULL )
    //{
    //	GLuint *indexBuf = (*indices);
    //	for ( i = 0; i < numParallels ; i++ )
    //	{
    //		for ( j = 0; j < numSlices; j++ )
    //		{
    //			*indexBuf++  = i * ( numSlices + 1 ) + j;
    //			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + j;
    //			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );

    //			*indexBuf++ = i * ( numSlices + 1 ) + j;
    //			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );
    //			*indexBuf++ = i * ( numSlices + 1 ) + ( j + 1 );
    //		}
    //	}
    //}

    //return numIndices;
    int slicesOver2 = numSlices / 2;
    int slicesOver2Minus1 = slicesOver2 - 1;
    int slicesOver2Minus2 = slicesOver2 - 2;
    delete[] *normals;
    GLfloat* n = *normals = (GLfloat*)malloc((numSlices * (slicesOver2Minus1)+2) * 3 * sizeof(GLfloat));
    n[0] = 0.0f;
    n[1] = 0.0f;
    n[2] = 1.0f;
    int countNormals = 1;
    for (int i = 1; i < slicesOver2; i++) {
        double latitude = M_PI * i / slicesOver2;
        double sineOfLatitude = sin(latitude);
        for (int j = 0; j < numSlices; j++) {
            double longitude = 2.0 * M_PI * j / numSlices;
            n[3 * countNormals] = cos(longitude) * sineOfLatitude;
            n[3 * countNormals + 1] = sin(longitude) * sineOfLatitude;
            n[3 * countNormals + 2] = cos(latitude);
            countNormals++;
        }
    }
    n[3 * countNormals] = 0.0f;
    n[3 * countNormals + 1] = 0.0f;
    n[3 * countNormals + 2] = -1.0f;
    free(*vertices);
    GLfloat* v = *vertices = (GLfloat*)malloc((numSlices * (slicesOver2Minus1)+2) * 3 * sizeof(GLfloat));
    for (int i = 0; i < (numSlices * (slicesOver2 - 1) + 2) * 3; i++) {
        v[i] = radius * n[i];
    }
    free(*indices);
    GLuint* index = *indices = (GLuint*)malloc(numSlices * (slicesOver2 - 1) * 6 * sizeof(GLuint));
    int countIndices = 0;
    for (int i = 0; i < numSlices; i++) {
        index[countIndices] = 0;					   countIndices++;
        index[countIndices] = i + 1;				   countIndices++;
        index[countIndices] = (i + 1) % numSlices + 1;	countIndices++;
    }
    for (int i = 0; i < slicesOver2Minus2; i++) {
        for (int j = 0; j < numSlices; j++) {
            index[countIndices] = i * numSlices + j + 1;							   countIndices++;
            index[countIndices] = i * numSlices + j + 1 + numSlices;		countIndices++;
            index[countIndices] = i * numSlices + (j + 1) % numSlices + numSlices + 1;	countIndices++;
            index[countIndices] = i * numSlices + j + 1;							   countIndices++;
            index[countIndices] = i * numSlices + (j + 1) % numSlices + numSlices + 1;	countIndices++;
            index[countIndices] = i * numSlices + (j + 1) % numSlices + 1;	countIndices++;
        }
    }
    for (int i = 0; i < numSlices; i++) {
        index[countIndices] = numSlices * slicesOver2Minus2 + 1 + i;				   countIndices++;
        index[countIndices] = numSlices * slicesOver2Minus1 + 1;					   countIndices++;
        index[countIndices] = numSlices * slicesOver2Minus2 + (1 + i) % numSlices + 1;	countIndices++;
    }
    return countIndices;
}

//
/// \brief Generates geometry for a cube.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLES
/// \param scale The size of the cube, use 1.0 for a unit cube.
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLES
//

//int ESUTIL_API esGenCube ( float scale, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint **indices );
int ESUTIL_API esGenCube(float scale, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices)
{
    int i;
    int numVertices = 24;
    int numIndices = 36;

    GLfloat cubeVerts[] =
            {
                    -0.5f, -0.5f, -0.5f,
                    -0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f, -0.5f,
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f, -0.5f,
                    -0.5f, -0.5f, 0.5f,
                    -0.5f,  0.5f, 0.5f,
                    0.5f,  0.5f, 0.5f,
                    0.5f, -0.5f, 0.5f,
                    -0.5f, -0.5f, -0.5f,
                    -0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f, -0.5f,
            };

    GLfloat cubeNormals[] =
            {
                    0.0f, -1.0f, 0.0f,
                    0.0f, -1.0f, 0.0f,
                    0.0f, -1.0f, 0.0f,
                    0.0f, -1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, -1.0f,
                    0.0f, 0.0f, -1.0f,
                    0.0f, 0.0f, -1.0f,
                    0.0f, 0.0f, -1.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    -1.0f, 0.0f, 0.0f,
                    -1.0f, 0.0f, 0.0f,
                    -1.0f, 0.0f, 0.0f,
                    -1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
            };

    GLfloat cubeTex[] =
            {
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    1.0f, 0.0f,
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    1.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    1.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    1.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    1.0f, 0.0f,
            };

    // Allocate memory for buffers
    if (vertices != NULL)
    {
        *vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
        memcpy(*vertices, cubeVerts, sizeof(cubeVerts));
        for (i = 0; i < numVertices * 3; i++)
        {
            (*vertices)[i] *= scale;
        }
    }

    if (normals != NULL)
    {
        *normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
        memcpy(*normals, cubeNormals, sizeof(cubeNormals));
    }

    if (texCoords != NULL)
    {
        *texCoords = (GLfloat*)malloc(sizeof(GLfloat) * 2 * numVertices);
        memcpy(*texCoords, cubeTex, sizeof(cubeTex));
    }


    // Generate the indices
    if (indices != NULL)
    {
        GLuint cubeIndices[] =
                {
                        0, 2, 1,
                        0, 3, 2,
                        4, 5, 6,
                        4, 6, 7,
                        8, 9, 10,
                        8, 10, 11,
                        12, 15, 14,
                        12, 14, 13,
                        16, 17, 18,
                        16, 18, 19,
                        20, 23, 22,
                        20, 22, 21
                };

        *indices = (GLuint*)malloc(sizeof(GLuint) * numIndices);
        memcpy(*indices, cubeIndices, sizeof(cubeIndices));
    }

    return numIndices;
}

//
/// \brief Generates a square grid consisting of triangles.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list as TRIANGLES.
/// \param size create a grid of size by size (number of triangles = (size-1)*(size-1)*2)
/// \param vertices If not NULL, will contain array of float3 positions
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLES
//

//int ESUTIL_API esGenSquareGrid ( int size, GLfloat **vertices, GLuint **indices );
int ESUTIL_API esGenSquareGrid(int size, GLfloat** vertices, GLuint** indices)
{
    int i, j;
    int numIndices = (size - 1) * (size - 1) * 2 * 3;

    // Allocate memory for buffers
    if (vertices != NULL)
    {
        int numVertices = size * size;
        float stepSize = (float)size - 1;
        *vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);

        for (i = 0; i < size; ++i) // row
        {
            for (j = 0; j < size; ++j) // column
            {
                (*vertices)[3 * (j + i * size)] = i / stepSize;
                (*vertices)[3 * (j + i * size) + 1] = j / stepSize;
                (*vertices)[3 * (j + i * size) + 2] = 0.0f;
            }
        }
    }

    // Generate the indices
    if (indices != NULL)
    {
        *indices = (GLuint*)malloc(sizeof(GLuint) * numIndices);

        for (i = 0; i < size - 1; ++i)
        {
            for (j = 0; j < size - 1; ++j)
            {
                // two triangles per quad
                (*indices)[6 * (j + i * (size - 1))] = j + (i) * (size);
                (*indices)[6 * (j + i * (size - 1)) + 1] = j + (i) * (size)+1;
                (*indices)[6 * (j + i * (size - 1)) + 2] = j + (i + 1) * (size)+1;

                (*indices)[6 * (j + i * (size - 1)) + 3] = j + (i) * (size);
                (*indices)[6 * (j + i * (size - 1)) + 4] = j + (i + 1) * (size)+1;
                (*indices)[6 * (j + i * (size - 1)) + 5] = j + (i + 1) * (size);
            }
        }
    }

    return numIndices;
}

//
/// \brief Loads a 8-bit, 24-bit or 32-bit TGA image from a file
/// \param ioContext Context related to IO facility on the platform
/// \param fileName Name of the file on disk
/// \param width Width of loaded image in pixels
/// \param height Height of loaded image in pixels
///  \return Pointer to loaded image.  NULL on failure.
//

//char *ESUTIL_API esLoadTGA ( void *ioContext, const char *fileName, int *width, int *height );
//char* ESUTIL_API esLoadTGA ( char *fileName, int *width, int *height )
//{
//	char *buffer;
//
//	if ( WinTGALoad ( fileName, &buffer, width, height ) )
//	{
//		return buffer;
//	}
//
//	return NULL;
//}


//
/// \brief Multiply matrix specified by result with a scaling matrix and return new matrix in result
/// \param result Specifies the input matrix.  Scaled matrix is returned in result.
/// \param sx, sy, sz Scale factors along the x, y and z axes respectively
//

//void ESUTIL_API esScale ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
void ESUTIL_API esScale(ESMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

//
/// \brief Multiply matrix specified by result with a translation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Translated matrix is returned in result.
/// \param tx, ty, tz Scale factors along the x, y and z axes respectively
//

//void ESUTIL_API esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
void ESUTIL_API esTranslate(ESMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

//
/// \brief Multiply matrix specified by result with a rotation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Rotated matrix is returned in result.
/// \param angle Specifies the angle of rotation, in degrees.
/// \param x, y, z Specify the x, y and z coordinates of a vector, respectively
//

//void ESUTIL_API esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
void ESUTIL_API esRotate(ESMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle, cosAngle;
    GLfloat mag = sqrtf(x * x + y * y + z * z);

    sinAngle = sinf(angle * M_PI / 180.0f);
    cosAngle = cosf(angle * M_PI / 180.0f);
    if (mag > 0.0f)
    {
        GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
        GLfloat oneMinusCos;
        ESMatrix rotMat;

        x /= mag;
        y /= mag;
        z /= mag;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * sinAngle;
        ys = y * sinAngle;
        zs = z * sinAngle;
        oneMinusCos = 1.0f - cosAngle;

        rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.m[0][1] = (oneMinusCos * xy) - zs;
        rotMat.m[0][2] = (oneMinusCos * zx) + ys;
        rotMat.m[0][3] = 0.0F;

        rotMat.m[1][0] = (oneMinusCos * xy) + zs;
        rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.m[1][2] = (oneMinusCos * yz) - xs;
        rotMat.m[1][3] = 0.0F;

        rotMat.m[2][0] = (oneMinusCos * zx) - ys;
        rotMat.m[2][1] = (oneMinusCos * yz) + xs;
        rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.m[2][3] = 0.0F;

        rotMat.m[3][0] = 0.0F;
        rotMat.m[3][1] = 0.0F;
        rotMat.m[3][2] = 0.0F;
        rotMat.m[3][3] = 1.0F;

        esMatrixMultiply(result, &rotMat, result);
    }
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  Both distances must be positive.
//

//void ESUTIL_API esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void ESUTIL_API esFrustum(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    frust;

    if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
        (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
        return;

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    esMatrixMultiply(result, &frust, result);
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param fovy Field of view y angle in degrees
/// \param aspect Aspect ratio of screen
/// \param nearZ Near plane distance
/// \param farZ Far plane distance
//

//void ESUTIL_API esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );
void ESUTIL_API esPerspective(ESMatrix* result, float fovy, float aspect, float nearZ, float farZ)
{
    GLfloat frustumW, frustumH;

    frustumH = tanf(fovy / 360.0f * M_PI) * nearZ;
    frustumW = frustumH * aspect;

    esFrustum(result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  These values are negative if plane is behind the viewer
//

//void ESUTIL_API esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void ESUTIL_API esOrtho(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    ortho;

    if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
        return;

    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    esMatrixMultiply(result, &ortho, result);
}

//
/// \brief Perform the following operation - result matrix = srcA matrix * srcB matrix
/// \param result Returns multiplied matrix
/// \param srcA, srcB Input matrices to be multiplied
//

//void ESUTIL_API esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
void ESUTIL_API esMatrixMultiply(ESMatrix* result, ESMatrix* srcA, ESMatrix* srcB)
{
    ESMatrix    tmp;
    int         i;

    for (i = 0; i < 4; i++)
    {
        tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) +
                      (srcA->m[i][1] * srcB->m[1][0]) +
                      (srcA->m[i][2] * srcB->m[2][0]) +
                      (srcA->m[i][3] * srcB->m[3][0]);

        tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) +
                      (srcA->m[i][1] * srcB->m[1][1]) +
                      (srcA->m[i][2] * srcB->m[2][1]) +
                      (srcA->m[i][3] * srcB->m[3][1]);

        tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) +
                      (srcA->m[i][1] * srcB->m[1][2]) +
                      (srcA->m[i][2] * srcB->m[2][2]) +
                      (srcA->m[i][3] * srcB->m[3][2]);

        tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) +
                      (srcA->m[i][1] * srcB->m[1][3]) +
                      (srcA->m[i][2] * srcB->m[2][3]) +
                      (srcA->m[i][3] * srcB->m[3][3]);
    }
    memcpy(result, &tmp, sizeof(ESMatrix));
}

//
//// \brief Return an identity matrix
//// \param result Returns identity matrix
//

//void ESUTIL_API esMatrixLoadIdentity ( ESMatrix *result );
void ESUTIL_API esMatrixLoadIdentity(ESMatrix* result)
{
    memset(result, 0x0, sizeof(ESMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

//
/// \brief Generate a transformation matrix from eye position, look at and up vectors
/// \param result Returns transformation matrix
/// \param posX, posY, posZ           eye position
/// \param lookAtX, lookAtY, lookAtZ  look at vector
/// \param upX, upY, upZ              up vector
//
void ESUTIL_API esMatrixLookAt(ESMatrix* result, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ) {

}

using std::string;

#define STR(s) #s
#define STRV(s) STR(s)
#define POSITION_ATTRIBUTE_LOCATION 0
#define NORMAL_ATTRIBUTE_LOCATION 1
#define UV_ATTRIBUTE_LOCATION 2

class Shader {
private:
public:
    GLuint mProgram;
    GLuint mvpMatrixLocation;
    GLuint cameraPositionLocation;
    Shader() {

    }
    Shader(char vertexShaderString[], char fragmentShaderString[]) {
        mProgram = esLoadProgram(vertexShaderString, fragmentShaderString);
        mvpMatrixLocation = glGetUniformLocation(mProgram, "u_mvpMatrix");
        cameraPositionLocation = glGetUniformLocation(mProgram, "cameraPosition");
    }
    void useProgram(ESMatrix mvpMatrix) {
        glUseProgram(mProgram);
        glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (GLfloat*)&mvpMatrix);
        //	setUniforms();
    }
    void setUniform(const char* name, unsigned int value) {
        glUniform1ui(glGetUniformLocation(mProgram, name), value);
    }
    void setUniform(const char* name, int value) {
        glUniform1i(glGetUniformLocation(mProgram, name), value);
    }
    void setUniform(const char* name, float value) {
        glUniform1f(glGetUniformLocation(mProgram, name), value);
    }
    void setUniform(const char* name, ESMatrix mvpMatrix) {
        glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (GLfloat*)&mvpMatrix);
    }
    void setUniform(const char* name, vec2 value) {
        glUniform2f(glGetUniformLocation(mProgram, name), value.x, value.y);
    }
    void setUniform(const char* name, ivec2 value) {
        glUniform2i(glGetUniformLocation(mProgram, name), value.x, value.y);
    }
    void setUniform(const char* name, vec4 value) {
        glUniform4f(glGetUniformLocation(mProgram, name), value.x, value.y, value.z, value.w);
    }
};

GLfloat box[] = {
        // FRONT
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // BACK
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        // LEFT
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        // RIGHT
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // TOP
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        // BOTTOM
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,
};

Shader basic;

char vertexShader[] =
        "#version 320 es\n"
        "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
        "uniform mat4 u_mvpMatrix;\n"
        "void main() {\n"
        "    gl_Position = u_mvpMatrix*vec4(pos, 1.0);\n"
        "}\n"
;

char fragmentShader[] =
        "#version 320 es\n"
        "precision mediump float;\n"
        "uniform vec4 color;\n"
        "layout(location = 0) out vec4 outColor;             \n"
        "void main() {\n"
        "    outColor = color;\n"
        "}\n"
;

void placeholder(GLuint program){
    glUseProgram(program);
}

class {
public:
    void gluPerspectivef(float maxViewAngle, float aspect, float zNear, float zFar){
        esMatrixLoadIdentity(&projection);
        esPerspective(&projection, maxViewAngle, aspect, zNear, zFar);
    }
    void glLoadIdentity(){
        esMatrixLoadIdentity(&transformation);
        passMVPtoGPU();
    }
    void glTranslatef(float x, float y, float z){
        esTranslate(&transformation, x, y, z);
        passMVPtoGPU();
    }
    void glRotatef(float angle, float x, float y, float z){
        esRotate(&transformation, angle, x, y, z);
        passMVPtoGPU();
    }
    void glScalef(float x, float y, float z){
        esScale(&transformation, x, y, z);
        passMVPtoGPU();
    }
    void glColor4f(float r, float g, float b, float a){
        glUniform4f(glGetUniformLocation(basic.mProgram, "color"), r, g, b, a);
    }
    void glPushMatrix(){
        transformationStack.push(transformation);
    }
    void glPopMatrix(){
        if(!transformationStack.empty()){
            ESMatrix* top = &transformationStack.top();
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    transformation.m[i][j] = top->m[i][j];
                }
            }
            transformationStack.pop();
        }
        passMVPtoGPU();
    }
    void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void * pointer){
        if(!shadersInitialized){
            initializeShaders();
            glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
        }
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, size, type, GL_FALSE, stride, (const GLvoid*)pointer);
    }
    void glNormalPointer(GLint size, GLenum type, GLsizei stride, const void * pointer){
        if(!shadersInitialized){
            initializeShaders();
            glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        }
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, size, type, GL_FALSE, stride, (const GLvoid*)pointer);
    }
    void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void * pointer){
        if(!shadersInitialized){
            initializeShaders();
            glEnableVertexAttribArray(UV_ATTRIBUTE_LOCATION);
        }
        glVertexAttribPointer(UV_ATTRIBUTE_LOCATION, size, type, GL_FALSE, stride, (const GLvoid*)pointer);
    }
    void glUseProgram(GLuint program){
        placeholder(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvpMatrix"), 1, GL_FALSE, (GLfloat*)&mvpMatrix);
    }
    ESMatrix rotationVectorTo4by4Matrix(const vec4& rotationVector) {
        mat3 matrix = quaternionTo3x3(rotationVector);
        ESMatrix _return;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                _return.m[i][j] = matrix.m[i][j];
            }
            _return.m[i][3] = 0.0f;
            _return.m[3][i] = 0.0f;
        }
        _return.m[3][3] = 1.0f;
        return _return;
    }
    void rotateTransformationMatrix(const vec4& rotationVectorInput) {
        ESMatrix rotationVector4by4Matrix = rotationVectorTo4by4Matrix(rotationVectorInput);
        ESMatrix copyOfTransformation = transformation;
        esMatrixMultiply(&transformation, &rotationVector4by4Matrix, &copyOfTransformation);
    }
    void drawLine(vec3 start, vec3 end){
        float vertices[] = {
                start.x, start.y, start.z,
                end.x, end.y, end.z
        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
    }
    void drawTriangle(vec3 start, vec3 mid, vec3 end){
        float vertices[] = {
                start.x, start.y, start.z,
                mid.x, mid.y, mid.z,
                end.x, end.y, end.z
        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void drawCube(float x, float y, float z){
        glVertexPointer(3, GL_FLOAT, 0, box);

        glPushMatrix();

        glTranslatef(x, y, z);

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        glPopMatrix();
    }
    void drawCube(vec3 position){
        drawCube(position.x, position.y, position.z);
    }
    void drawCubeColored(float x, float y, float z, vec4 color){
        glVertexPointer(3, GL_FLOAT, 0, box);

        glPushMatrix();

        glTranslatef(x, y, z);

        glColor4f(color.r, color.g, color.b, color.a);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        glPopMatrix();
    }
    void drawCubeColored(vec3 position, vec4 color){
        drawCubeColored(position.x, position.y, position.z, color);
    }
    bool initializeShaders(){
        basic = Shader(vertexShader, fragmentShader);
        shadersInitialized = true;
        return true;
    }
    ESMatrix getMVP(){
        return mvpMatrix;
    }
private:
    bool shadersInitialized = false;
    ESMatrix transformation;
    ESMatrix projection;
    ESMatrix mvpMatrix;
    std::stack <ESMatrix> transformationStack;
    void passMVPtoGPU(){
        esMatrixMultiply(&mvpMatrix, &transformation, &projection);
        basic.useProgram(mvpMatrix);
    }
} gl;

// Our saved state data.
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine
{
    struct android_app* app;

    /*ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    const ASensor* rotationVectorSensor;
    ASensorEventQueue* accelerometerSensorEventQueue;
    ASensorEventQueue* rotationVectorSensorEventQueue;

    int animating = 1;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;*/
    //EGLint width;
    //EGLint height;
    vec3 accelerometerVectorInput;
    vec4 rotationVectorInput;
    int framesDrawn = 0;
    struct saved_state state;
};
struct engine* engine;

void onDrawFrame(float acc_x, float acc_y, float acc_z, float rot_x, float rot_y, float rot_z, float rot_w){
    //engine->accelerometerVectorInput = vec3(acc_x, acc_y, acc_z);
    //engine->rotationVectorInput = vec4(rot_x, rot_y, rot_z, rot_w);
    if(initiated) {
        //if (engine->display == NULL)
        {
            // No display.
            //    return;
        }

        // Set the viewport
        glViewport(0, 0, screen_width, screen_height);
        // Compute the window aspect ratio
        GLfloat aspect = (GLfloat) screen_width / (GLfloat) screen_height;
        // Generate a perspective matrix with a 90-degree FOV and near and far clip planes at 0.1 and 1000.0
        float maxViewAngle = 45.0f;
        float zNear = 0.1f;
        float zFar = 1000.0f;
        float verticalScreenAngle = (aspect < 1.0f) ? maxViewAngle : 2.0 * atan(tan(
                maxViewAngle / 2.0 * M_PI / 180.0) / aspect) / M_PI * 180.0;
        gl.gluPerspectivef(verticalScreenAngle, aspect, zNear, zFar);

        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl.glLoadIdentity();

        vec3 cameraPosition =
                quaternionTo3x3(engine->rotationVectorInput) * vec3(0.0f, 0.0f, 10.0f);
        /*gl.rotateTransformationMatrix(engine->rotationVectorInput);
        gl.glTranslatef(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);

        gl.drawCube(vec3(0.0f, 0.0f, 0.0f));
        gl.drawCube(vec3(0.0f, 0.0f, 2.0f));

        engine->framesDrawn++;*/
    }
}
void onSurfaceChanged(int width, int height){
    screen_width = width;
    screen_height = height;
}
void onSurfaceCreated(){
    gl.initializeShaders();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnableVertexAttribArray ( POSITION_ATTRIBUTE_LOCATION );

    initiated = true;
}

#endif