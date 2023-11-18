#include <jni.h>
#include <string>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stack>
#include <android/log.h>
#include <cmath>
#include <EGL/egl.h>
#include "vectors.h"
#include "cyCodeBase-master/cyMatrix.h"

using cy::Matrix4;
using cy::Vec3;
using std::min;
using std::max;

//#define DYNAMIC_ES3 true
#if DYNAMIC_ES3
#include "gl3stub.h"
#else
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

#endif

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

class Wallpaper{
public:

    int framesRendered;

    EGLContext mEglContext;

    float val;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    struct Vertex {
        GLfloat pos[3];
    };

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

    virtual bool initialize();

    virtual void render();

    static bool checkGlError(const char* funcName);

    static GLuint createShader(GLenum shaderType, const char* src);

    static GLuint createProgram(const char* vtxSrc, const char* fragSrc);

    static void printGlString(const char* name, GLenum s);

    void calculatePerspective();

private:

};

Wallpaper::Wallpaper() : framesRendered(0), val(0), mEglContext(eglGetCurrentContext()){

}

Wallpaper::~Wallpaper(){
    if (eglGetCurrentContext() != mEglContext) return;
}

bool Wallpaper::initialize(){

}

void Wallpaper::render(){

}

void Wallpaper::calculatePerspective() {
    perspective.SetPerspective(45.0f, (float) width / (float) height, 0.1f, 1000.0f);
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

GLuint Wallpaper::createShader(GLenum shaderType, const char* src) {
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

GLuint Wallpaper::createProgram(const char* vtxSrc, const char* fragSrc) {
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

void Wallpaper::printGlString(const char* name, GLenum s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

class Box : public Wallpaper{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    Box();

    ~Box();

    bool initialize();

    void render();

private:
};

Box::Box() : Wallpaper(){
    initialize();
}

Box::~Box(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
}

bool Box::initialize() {
    mProgram = 0;
    mVBState = 0;
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram) return false;

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, pos));

    return true;
}

void Box::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POS_ATTRIB);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, val * 1.0f - 3.0f));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> MVP = perspective * translation * rotation;
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&MVP.cell);
    glBindVertexArray(mVBState);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    glDisableVertexAttribArray(POS_ATTRIB);

    checkGlError("Renderer::render");
    framesRendered++;
}

class Graph : Wallpaper{

};

class Simulation : public Wallpaper{
public:
    int moleculeCount;
    int cbrtMoleculeCount;
    void setMoleculeCount(int n);
};

void Simulation::setMoleculeCount(int n){
    moleculeCount = n;
    cbrtMoleculeCount = cbrt(moleculeCount);
}

class BarnesHut : Simulation{

};

class BruteForce : Simulation{

};

class Naive : public Simulation{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    struct Molecule {
        vec3 position;
        vec3 velocity;
        vec3 positionFinal;
        vec3 sumForceInitial;
        vec3 sumForceFinal;
    };

    Molecule* molecules;
    bool seed(float radius);
    void simulate(vec3 gravity);

    Naive();

    ~Naive();

    bool initialize();

    void render();

private:

    const float deltaTime = 0.02f;

    const float surfaceTension = 0.05f;

    float sphereRadius;

    float sphereRadiusSquared;

    float sphereRadiusPlusPointFive;

    float sphereRadiusPlusPointFiveSquared;

    const float efficiency = 1.0f;//0.995f;

    const float maxForce = 100000000.0f;

    const float maxForceSquared = maxForce * maxForce;

    const double maxForceDistance = 2.0;

    const double maxForceDistanceSquared = maxForceDistance * maxForceDistance;

    float particleRadius;

    int* numCellParticles;

    int* firstCellParticle;

    int* cellParticleIds;

    float pInvSpacing;

    int pNumX;

    int pNumY;

    int pNumZ;

    int pNumCells;

    void populateGrid(int dataIndex);

    void useGrid(int i, int dataIndex, int* ids, int& count);

    float noPullBarrier(float x);

    float f(float x);

    inline void calculateForce(int i, vec3& gravity, vec3& force, vec3& position, int dataIndex);
};

Naive::Naive() : Simulation(){

}

Naive::~Naive(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
    delete molecules;
}

bool Naive::initialize() {
    mProgram = 0;
    mVBState = 0;
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram) return false;

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, pos));

    seed(15.0f);

    return true;
}

void Naive::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POS_ATTRIB);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    Matrix4<float> translation2;
    for(int i = 0; i < moleculeCount; i++) {
        translation = translation.Translation(
                Vec3<float>(0.0f, 0.0f, val * 10.0f - 30.0f));
        translation2 = translation2.Translation(
                Vec3<float>(molecules[i].position[0], molecules[i].position[1], molecules[i].position[2]));
        Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
        Matrix4<float> MVP = perspective * translation * rotation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &MVP.cell);
        glBindVertexArray(mVBState);
        glUniform4f(glGetUniformLocation(mProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POS_ATTRIB);

    for(int i = 0; i < 5; i++){
        simulate(accelerometerVector);
    }

    checkGlError("Renderer::render");
    framesRendered++;
}

void Naive::populateGrid(int dataIndex){
    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        numCellParticles[cellNr]++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += numCellParticles[i];
        firstCellParticle[i] = first;
    }
    firstCellParticle[pNumCells] = first; // guard

    // fill particles into cells

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }
}

void Naive::useGrid(int i, int dataIndex, int* ids, int& count){
    count = 0;
    float px = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
    float py = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
    float pz = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

    int pxi = floor(px * pInvSpacing);
    int pyi = floor(py * pInvSpacing);
    int pzi = floor(pz * pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, pNumX - 1);
    int y1 = min(pyi + 1, pNumY - 1);
    int z1 = min(pzi + 1, pNumZ - 1);

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
                int first = firstCellParticle[cellNr];
                int last = firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = cellParticleIds[j];
                    if (id == i)
                        continue;
                    ids[count++] = id;
                }
            }
        }
    }
}

bool Naive::seed(float radius) {
    setMoleculeCount(125);
    molecules = new Molecule[moleculeCount];
    sphereRadius = radius;
    sphereRadiusSquared = sphereRadius * sphereRadius;
    sphereRadiusPlusPointFive = sphereRadius + 0.5f;
    sphereRadiusPlusPointFiveSquared = sphereRadiusPlusPointFive * sphereRadiusPlusPointFive;
    for (int i = 0; i < moleculeCount; i++) {
        int first = i - (i % (cbrtMoleculeCount * cbrtMoleculeCount));
        molecules[i].position.x = first / (cbrtMoleculeCount * cbrtMoleculeCount) - (cbrtMoleculeCount - 1) / 2.0f;
        first = i - first;
        molecules[i].position.y = (first - (first % cbrtMoleculeCount)) / cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].position.z = first % cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].velocity = vec3(0.0f, 0.000001f, 0.0f);
        molecules[i].positionFinal = molecules[i].position;
    }

    particleRadius = 0.5f;
    pInvSpacing = 1.0 / (maxForceDistance * 2.0 * particleRadius);//4.0 correlates to max distance of 2.0, 3.0 correlates to max distance of 1.5
    float width = 2.0f * sphereRadiusPlusPointFive;
    float height = 2.0f * sphereRadiusPlusPointFive;
    float depth = 2.0f * sphereRadiusPlusPointFive;
    pNumX = floor(width * pInvSpacing) + 1;
    pNumY = floor(height * pInvSpacing) + 1;
    pNumZ = floor(depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;
    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(moleculeCount, sizeof(int));

    populateGrid(0);

    return true;
}

float Naive::noPullBarrier(float x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    return 1000.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
}

float Naive::f(float x) {
    if (x > maxForceDistance) {
        return 0.0f;
    }
    float _return = 500.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
    if (x > 1.0f) {
        return surfaceTension * _return;
    } else {
        return _return;
    }
}

inline void Naive::calculateForce(int i, vec3& gravity, vec3& force, vec3& position, int dataIndex){
    force = gravity;
    int ids[1000]; // 700 may be high enough
    int count = 0;
    useGrid(i, dataIndex, ids, count);
    for (int j = 0; j < count; j++) {
        if (i == j) {
            continue;
        }
        vec3 difference = ((vec3*)&molecules[ids[j]])[dataIndex] - position;
        float r = distance(difference);
        float fOverR = f(r) / r;
        force += fOverR * difference;
    }
    float distanceFromOrigin = distance(position);
    if (distanceFromOrigin != 0.0f) {
        float forceAtPoint = noPullBarrier(-distanceFromOrigin + sphereRadiusPlusPointFive) / distanceFromOrigin;//if distanceFromOrigin = 0, then we plug in a value of 3.5, if distanceFromOrigin = 2.5, then we plug in a value of 1
        force += forceAtPoint * position;
    }
}

void Naive::simulate(vec3 gravity) {
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceInitial, molecules[i].position, 0);
        molecules[i].positionFinal = molecules[i].position + deltaTime * (molecules[i].sumForceInitial * 0.5f * deltaTime + molecules[i].velocity);
        // Bug fix: Stop runaway particles
        float squaredDistance = dot(molecules[i].positionFinal, molecules[i].positionFinal);
        if(squaredDistance > sphereRadiusPlusPointFive * sphereRadiusPlusPointFive){
            molecules[i].positionFinal *= sqrt(sphereRadius * sphereRadius / squaredDistance);
        }
    }
    populateGrid(2);
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceFinal, molecules[i].positionFinal, 2);
        vec3 sumForceAverage = 0.5f * (molecules[i].sumForceInitial + molecules[i].sumForceFinal);
        molecules[i].position += deltaTime * (sumForceAverage * 0.5f * deltaTime + molecules[i].velocity);
        molecules[i].velocity += sumForceAverage * deltaTime;
        // Efficiency control
        if (efficiency != 1.0f) {
            vec3 product = molecules[i].velocity * efficiency;
            molecules[i].velocity = product;
        }
        //  Bug fix: stop runaway particles
        float squaredDistance = dot(molecules[i].position, molecules[i].position);
        if(squaredDistance > sphereRadiusSquared){
            molecules[i].position *= sqrt(sphereRadiusSquared / squaredDistance);
            molecules[i].velocity = 0.1f * molecules[i].position;
        }
    }
    populateGrid(0);
}

class PicFlip : Simulation{

};

Wallpaper* wallpaper = nullptr;

// ----------------------------------------------------------------------------

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_livewallpaper05_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// ----------------------------------------------------------------------------

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() { return GL_TRUE; }
#endif

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_init(JNIEnv *env, jobject thiz) {
    Wallpaper::printGlString("Version", GL_VERSION);
    Wallpaper::printGlString("Vendor", GL_VENDOR);
    Wallpaper::printGlString("Renderer", GL_RENDERER);
    Wallpaper::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        wallpaper = new Naive();
        wallpaper->initialize();
        ALOGV("Using OpenGL ES 3.0 renderer");
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        //g_renderer = createES2Renderer();
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_resize(JNIEnv *env, jobject thiz,
                                                                    jint width, jint height) {
    if (wallpaper) {
        wallpaper->width = width;
        wallpaper->height = height;
        glViewport(0, 0, width, height);
        wallpaper->calculatePerspective();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w) {
    if (wallpaper) {
        wallpaper->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        wallpaper->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        wallpaper->render();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_sendData(JNIEnv *env, jobject thiz,
                                                                      jfloat value) {
    wallpaper->val = value;
}