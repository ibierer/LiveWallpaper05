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

class Graph : public Wallpaper{

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

class BarnesHut : public Simulation{

};

class BruteForce : public Simulation{

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
    translation = translation.Translation(
            10.0f * Vec3<float>(0.0f, 0.0f, 10.0f * val - 6.0f));
    Matrix4<float> translation2;
    for(int i = 0; i < moleculeCount; i++) {
        translation2 = translation2.Translation(
                Vec3<float>(molecules[i].position[0], molecules[i].position[1], molecules[i].position[2]));
        Matrix4<float> MVP = perspective * translation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &MVP.cell);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.06125f * molecules[i].velocity.x + 0.5f,
                -0.06125f * molecules[i].velocity.y + 0.5f,
                -0.06125f * molecules[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POS_ATTRIB);

    for(int i = 0; i < 5; i++){
        simulate(-accelerometerVector);
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

struct {
    const int width = 1000;
    const int height = 1000;
} canvas;

float simHeight = 3.0;
float cScale = canvas.height / simHeight;
float simWidth = canvas.width / cScale;
float simDepth = simHeight;  // Assuming the depth is the same as the height

const int FLUID_CELL = 0;
const int AIR_CELL = 1;
const int SOLID_CELL = 2;

float min(float a, float b)  // Updated function signature and parameter type
{
    if (a < b)
        return a;
    else
        return b;
}

// ----------------- start of simulator ------------------------------

class FlipFluid {
public:

    // fluid

    float density;
    int fNumX;
    int fNumY;
    int fNumZ;  // New variable for the z-axis
    float h;
    float fInvSpacing;
    int fNumCells;

    float* u;
    float* v;
    float* w;  // New variable for the z-axis velocity
    float* du;
    float* dv;
    float* dw;  // New variable for the z-axis velocity
    float* prevU;
    float* prevV;
    float* prevW;  // New variable for the z-axis velocity
    float* p;
    float* s;
    int* cellType;

    // particles

    int maxParticles;

    vec3* particlePos;
    vec3* particleColor;

    vec3* particleVel;

    float* particleDensity;
    float particleRestDensity;

    float particleRadius;
    float pInvSpacing;
    int pNumX;
    int pNumY;
    int pNumZ;  // New variable for the z-axis
    int pNumCells;

    int* numCellParticles;
    int* firstCellParticle;
    int* cellParticleIds;

    int numParticles;

    float dt = 1.0 / 60.0;
    float flipRatio = 0.9f;
    int numPressureIters = 50;
    int numParticleIters = 2;
    int frameNr = 0;
    float overRelaxation = 1.9f;
    bool compensateDrift = true;
    bool separateParticles = true;

    FlipFluid(float _density, float _width, float _height, float _depth, float _spacing, float _particleRadius, int _maxParticles);

private:
    void integrateParticles(float _dt, vec3 _gravity);

    void pushParticlesApart(float _numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(bool _toGrid, float _flipRatio);

    void solveIncompressibility(int _numIters, float _dt, float _overRelaxation, bool _compensateDrift);

    void updateParticleColors();

public:
    void simulate(float _dt, vec3 _gravity, float _flipRatio, int _numPressureIters, int _numParticleIters, float _overRelaxation, bool _compensateDrift, bool _separateParticles);
};

FlipFluid* fluid = NULL;

FlipFluid::FlipFluid(float _density, float _width, float _height, float _depth, float _spacing, float _particleRadius, int _maxParticles) {

    // fluid properties

    density = _density;
    fNumX = floor(_width / _spacing) + 1;
    fNumY = floor(_height / _spacing) + 1;
    fNumZ = floor(_depth / _spacing) + 1;
    h = max(max(_width / fNumX, _height / fNumY), _depth / fNumZ);
    fInvSpacing = 1.0 / h;
    fNumCells = fNumX * fNumY * fNumZ;

    u = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the x-direction
    v = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the y-direction
    w = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the z-direction
    du = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the x-direction
    dv = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the y-direction
    dw = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the z-direction
    prevU = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the x-direction
    prevV = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the y-direction
    prevW = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the z-direction
    p = (float*)calloc(fNumCells, sizeof(float));         // Initialize pressure
    s = (float*)calloc(fNumCells, sizeof(float));         // Initialize source term
    cellType = (int*)calloc(fNumCells, sizeof(int));        // Initialize cell type (fluid, air, solid)

    // particle properties

    maxParticles = _maxParticles;

    particlePos = (vec3*)calloc(maxParticles, sizeof(vec3));   // Initialize particle positions (x, y, z)
    particleColor = (vec3*)calloc(maxParticles, sizeof(vec3)); // Initialize particle color (for visualization)

    particleVel = (vec3*)calloc(maxParticles, sizeof(vec3));      // Initialize particle velocity (x, y, z)
    particleDensity = (float*)calloc(fNumCells, sizeof(float));         // Initialize particle density
    particleRestDensity = 0.0;                                            // Set the rest density of the particles

    particleRadius = _particleRadius;
    pInvSpacing = 1.0 / (2.2 * _particleRadius);
    pNumX = floor(_width * pInvSpacing) + 1;
    pNumY = floor(_height * pInvSpacing) + 1;
    pNumZ = floor(_depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;

    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(_maxParticles, sizeof(int));           // Initialize the particle ids

    numParticles = 0;                                                     // Set the initial number of particles to zero
}

void FlipFluid::integrateParticles(float _dt, vec3 _gravity)
{
    for (int i = 0; i < numParticles; i++) {
        particleVel[i] += _dt * _gravity;
        particlePos[i] += particleVel[i] * _dt;
    }
}

void FlipFluid::pushParticlesApart(float _numIters)
{
    float colorDiffusionCoeff = 0.001;

    // count particles per cell

    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

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

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }

    // push particles apart

    float minDist = 2.0 * particleRadius;
    float minDist2 = minDist * minDist;

    for (int iter = 0; iter < _numIters; iter++) {

        for (int i = 0; i < numParticles; i++) {
            float px = particlePos[i].x;
            float py = particlePos[i].y;
            float pz = particlePos[i].z;

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
                            float qx = particlePos[id].x;
                            float qy = particlePos[id].y;
                            float qz = particlePos[id].z;

                            float dx = qx - px;
                            float dy = qy - py;
                            float dz = qz - pz;
                            float d2 = dx * dx + dy * dy + dz * dz;
                            if (d2 > minDist2 || d2 == 0.0)
                                continue;
                            float d = sqrt(d2);
                            float s = 0.5 * (minDist - d) / d;
                            dx *= s;
                            dy *= s;
                            dz *= s;
                            particlePos[i].x -= dx;
                            particlePos[i].y -= dy;
                            particlePos[i].z -= dz;
                            particlePos[id].x += dx;
                            particlePos[id].y += dy;
                            particlePos[id] += dz;

                            // diffuse colors

                            for (int k = 0; k < 3; k++) {
                                float color0 = particleColor[i].v[k];
                                float color1 = particleColor[id].v[k];
                                float color = (color0 + color1) * 0.5;
                                particleColor[i].v[k] = color0 + (color - color0) * colorDiffusionCoeff;
                                particleColor[id].v[k] = color1 + (color - color1) * colorDiffusionCoeff;
                            }
                        }
                    }
                }
            }
        }
    }
}

void FlipFluid::handleParticleCollisions()
{
    float r = particleRadius;

    float minX = h + r;
    float maxX = (fNumX - 1) * h - r;
    float minY = h + r;
    float maxY = (fNumY - 1) * h - r;
    float minZ = h + r;
    float maxZ = (fNumZ - 1) * h - r;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        // wall collisions

        if (x < minX) {
            x = minX;
            particleVel[i].x = 0.0;
        }
        if (x > maxX) {
            x = maxX;
            particleVel[i].x = 0.0;
        }
        if (y < minY) {
            y = minY;
            particleVel[i].y = 0.0;
        }
        if (y > maxY) {
            y = maxY;
            particleVel[i].y = 0.0;
        }
        if (z < minZ) {
            z = minZ;
            particleVel[i].z = 0.0;
        }
        if (z > maxZ) {
            z = maxZ;
            particleVel[i].z = 0.0;
        }
        particlePos[i].x = x;
        particlePos[i].y = y;
        particlePos[i].z = z;
    }
}

void FlipFluid::updateParticleDensity()
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5 * h;

    float* d = particleDensity;

    for (int i = 0; i < fNumCells; i++)
        d[i] = 0.0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        x = clamp(x, h, (fNumX - 1) * h);
        y = clamp(y, h, (fNumY - 1) * h);
        z = clamp(z, h, (fNumZ - 1) * h);

        int x0 = floor((x - h2) * h1);
        float tx = ((x - h2) - x0 * h) * h1;
        int x1 = min(x0 + 1, fNumX - 2);

        int y0 = floor((y - h2) * h1);
        float ty = ((y - h2) - y0 * h) * h1;
        int y1 = min(y0 + 1, fNumY - 2);

        int z0 = floor((z - h2) * h1);
        float tz = ((z - h2) - z0 * h) * h1;
        int z1 = min(z0 + 1, fNumZ - 2);

        float sx = 1.0 - tx;
        float sy = 1.0 - ty;
        float sz = 1.0 - tz;

        if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z0] += sx * sy * sz;
        if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z0] += tx * sy * sz;
        if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z0] += tx * ty * sz;
        if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z0] += sx * ty * sz;
        if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z1] += sx * sy * tz;
        if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z1] += tx * sy * tz;
        if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z1] += tx * ty * tz;
        if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z1] += sx * ty * tz;
    }

    if (particleRestDensity == 0.0) {
        float sum = 0.0;
        int numFluidCells = 0;

        for (int i = 0; i < fNumCells; i++) {
            if (cellType[i] == FLUID_CELL) {
                sum += d[i];
                numFluidCells++;
            }
        }

        if (numFluidCells > 0)
            particleRestDensity = sum / numFluidCells;
    }
}

void FlipFluid::transferVelocities(bool _toGrid, float _flipRatio)
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5f * h;

    if (_toGrid) {
        for (int i = 0; i < fNumCells; i++) {
            prevU[i] = u[i];
            prevV[i] = v[i];
            prevW[i] = w[i];
        }

        for (int i = 0; i < fNumCells; i++) {
            du[i] = 0.0;
            dv[i] = 0.0;
            dw[i] = 0.0;
            u[i] = 0.0;
            v[i] = 0.0;
            w[i] = 0.0;
        }

        for (int i = 0; i < fNumCells; i++)
            cellType[i] = s[i] == 0.0 ? SOLID_CELL : AIR_CELL;

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            int xi = clamp(floor(x * h1), 0, fNumX - 1);
            int yi = clamp(floor(y * h1), 0, fNumY - 1);
            int zi = clamp(floor(z * h1), 0, fNumZ - 1);

            int cellNr = xi * (n * fNumZ) + yi * fNumZ + zi;
            if (cellType[cellNr] == AIR_CELL)
                cellType[cellNr] = FLUID_CELL;
        }
    }

    for (int component = 0; component < 3; component++) {
        float dx = (component == 0) ? 0.0 : h2;
        float dy = (component == 1) ? 0.0 : h2;
        float dz = (component == 2) ? 0.0 : h2;

        float* f = (component == 0) ? u : ((component == 1) ? v : w);
        float* prevF = (component == 0) ? prevU : ((component == 1) ? prevV : prevW);
        float* d = (component == 0) ? du : ((component == 1) ? dv : dw);

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            x = clamp(x, h, (fNumX - 1) * h);
            y = clamp(y, h, (fNumY - 1) * h);
            z = clamp(z, h, (fNumZ - 1) * h);

            int x0 = min(floor((x - dx) * h1), fNumX - 2);
            float tx = ((x - dx) - x0 * h) * h1;
            int x1 = min(x0 + 1, fNumX - 2);

            int y0 = min(floor((y - dy) * h1), fNumY - 2);
            float ty = ((y - dy) - y0 * h) * h1;
            int y1 = min(y0 + 1, fNumY - 2);

            int z0 = min(floor((z - dz) * h1), fNumZ - 2);
            float tz = ((z - dz) - z0 * h) * h1;
            int z1 = min(z0 + 1, fNumZ - 2);

            float sx = 1.0 - tx;
            float sy = 1.0 - ty;
            float sz = 1.0 - tz;

            float d0 = sx * sy * sz;
            float d1 = tx * sy * sz;
            float d2 = tx * ty * sz;
            float d3 = sx * ty * sz;
            float d4 = sx * sy * tz;
            float d5 = tx * sy * tz;
            float d6 = tx * ty * tz;
            float d7 = sx * ty * tz;

            int nr0 = x0 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr1 = x1 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr2 = x1 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr3 = x0 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr4 = x0 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr5 = x1 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr6 = x1 * (n * fNumZ) + y1 * fNumZ + z1;
            int nr7 = x0 * (n * fNumZ) + y1 * fNumZ + z1;

            if (_toGrid) {
                float pv = particleVel[i].v[component];
                f[nr0] += pv * d0; d[nr0] += d0;
                f[nr1] += pv * d1; d[nr1] += d1;
                f[nr2] += pv * d2; d[nr2] += d2;
                f[nr3] += pv * d3; d[nr3] += d3;
                f[nr4] += pv * d4; d[nr4] += d4;
                f[nr5] += pv * d5; d[nr5] += d5;
                f[nr6] += pv * d6; d[nr6] += d6;
                f[nr7] += pv * d7; d[nr7] += d7;
            }
            else {
                int offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1);

                float valid0 = (cellType[nr0] != AIR_CELL || cellType[nr0 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid1 = (cellType[nr1] != AIR_CELL || cellType[nr1 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid2 = (cellType[nr2] != AIR_CELL || cellType[nr2 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid3 = (cellType[nr3] != AIR_CELL || cellType[nr3 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid4 = (cellType[nr4] != AIR_CELL || cellType[nr4 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid5 = (cellType[nr5] != AIR_CELL || cellType[nr5 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid6 = (cellType[nr6] != AIR_CELL || cellType[nr6 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid7 = (cellType[nr7] != AIR_CELL || cellType[nr7 - offset] != AIR_CELL) ? 1.0 : 0.0;

                float v = particleVel[i].v[component];
                float d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                          valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (d > 0.0) {
                    float picV = (valid0 * d0 * f[nr0] + valid1 * d1 * f[nr1] + valid2 * d2 * f[nr2] + valid3 * d3 * f[nr3] +
                                  valid4 * d4 * f[nr4] + valid5 * d5 * f[nr5] + valid6 * d6 * f[nr6] + valid7 * d7 * f[nr7]) / d;
                    float corr = (valid0 * d0 * (f[nr0] - prevF[nr0]) + valid1 * d1 * (f[nr1] - prevF[nr1]) +
                                  valid2 * d2 * (f[nr2] - prevF[nr2]) + valid3 * d3 * (f[nr3] - prevF[nr3]) +
                                  valid4 * d4 * (f[nr4] - prevF[nr4]) + valid5 * d5 * (f[nr5] - prevF[nr5]) +
                                  valid6 * d6 * (f[nr6] - prevF[nr6]) + valid7 * d7 * (f[nr7] - prevF[nr7])) / d;
                    float flipV = v + corr;

                    particleVel[i].v[component] = (1.0 - _flipRatio) * picV + _flipRatio * flipV;
                }
            }
        }

        if (_toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (d[i] > 0.0)
                    f[i] /= d[i];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int solid = cellType[i * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL;
                        if (solid || (i > 0 && cellType[(i - 1) * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL))
                            u[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (j > 0 && cellType[i * (n * fNumZ) + (j - 1) * fNumZ + k] == SOLID_CELL))
                            v[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (k > 0 && cellType[i * (n * fNumZ) + j * fNumZ + (k - 1)] == SOLID_CELL))
                            w[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                    }
                }
            }
        }
    }
}

void FlipFluid::solveIncompressibility(int _numIters, float _dt, float _overRelaxation, bool _compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        p[i] = 0.0;
        prevU[i] = u[i];
        prevV[i] = v[i];
        prevW[i] = w[i];
    }

    int n = fNumY;
    float cp = density * h / _dt;

    for (int iter = 0; iter < _numIters; iter++) {
        for (int i = 1; i < fNumX - 1; i++) {
            for (int j = 1; j < fNumY - 1; j++) {
                for (int k = 1; k < fNumZ - 1; k++) {

                    if (cellType[i * (n * fNumZ) + j * fNumZ + k] != FLUID_CELL)
                        continue;

                    int center = i * (n * fNumZ) + j * fNumZ + k;
                    int left = (i - 1) * (n * fNumZ) + j * fNumZ + k;
                    int right = (i + 1) * (n * fNumZ) + j * fNumZ + k;
                    int bottom = i * (n * fNumZ) + (j - 1) * fNumZ + k;
                    int top = i * (n * fNumZ) + (j + 1) * fNumZ + k;
                    int back = i * (n * fNumZ) + j * fNumZ + (k - 1);
                    int front = i * (n * fNumZ) + j * fNumZ + (k + 1);

                    float sx0 = s[left];
                    float sx1 = s[right];
                    float sy0 = s[bottom];
                    float sy1 = s[top];
                    float sz0 = s[back];
                    float sz1 = s[front];
                    float s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (s == 0.0)
                        continue;

                    float div = u[right] - u[center] +
                                v[top] - v[center] +
                                w[front] - w[center];

                    if (particleRestDensity > 0.0 && _compensateDrift) {
                        float _k = 1.0;
                        float compression = particleDensity[i * (n * fNumZ) + j * fNumZ + k] - particleRestDensity;
                        if (compression > 0.0)
                            div = div - _k * compression;
                    }

                    float p = -div / s;
                    p *= _overRelaxation;
                    this->p[center] += cp * p;

                    u[center] -= sx0 * p;
                    u[right] += sx1 * p;
                    v[center] -= sy0 * p;
                    v[top] += sy1 * p;
                    w[center] -= sz0 * p;
                    w[front] += sz1 * p;
                }
            }
        }
    }
}

void FlipFluid::updateParticleColors()
{
    float h1 = fInvSpacing;

    for (int i = 0; i < numParticles; i++) {
        float s = 0.01;

        particleColor[i].r = clamp(particleColor[i].r - s, 0.0, 1.0);
        particleColor[i].g = clamp(particleColor[i].g - s, 0.0, 1.0);
        particleColor[i].b = clamp(particleColor[i].b + s, 0.0, 1.0);

        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;
        int xi = clamp(floor(x * h1), 1, fNumX - 1);
        int yi = clamp(floor(y * h1), 1, fNumY - 1);
        int zi = clamp(floor(z * h1), 1, fNumZ - 1);
        int cellNr = xi * fNumY * fNumZ + yi * fNumZ + zi;

        float d0 = particleRestDensity;

        if (d0 > 0.0) {
            float relDensity = particleDensity[cellNr] / d0;
            if (relDensity < 0.7) {
                float s = 0.8;
                particleColor[i].r = s;
                particleColor[i].g = s;
                particleColor[i].b = 1.0;
            }
        }
    }
}

void FlipFluid::simulate(float _dt, vec3 _gravity, float _flipRatio, int _numPressureIters, int _numParticleIters, float _overRelaxation, bool _compensateDrift, bool _separateParticles)
{
    int numSubSteps = 1;
    float sdt = _dt / numSubSteps;

    for (int step = 0; step < numSubSteps; step++) {
        this->integrateParticles(sdt, _gravity);
        if (_separateParticles)
            this->pushParticlesApart(_numParticleIters);
        this->handleParticleCollisions();
        this->transferVelocities(true, _flipRatio);
        this->updateParticleDensity();
        this->solveIncompressibility(_numPressureIters, sdt, _overRelaxation, _compensateDrift);
        this->transferVelocities(false, _flipRatio);
    }

    this->updateParticleColors();
}

class PicFlip : public Simulation{
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

    bool initialize();

    void render();
    float* p;
    float* s;

    PicFlip();

    ~PicFlip();

public:

    static void simulate(const vec3& acceleration){
        fluid->simulate(
                fluid->dt, acceleration, fluid->flipRatio, fluid->numPressureIters, fluid->numParticleIters,
                fluid->overRelaxation, fluid->compensateDrift, fluid->separateParticles);
        fluid->frameNr++;
    }

    static void setupScene(){
        int res = 15;

        float tankHeight = 1.0f * simHeight;
        float tankWidth = 1.0f * simWidth;
        float tankDepth = 1.0f * simDepth;
        float h = tankHeight / res;
        float density = 1000.0f;

        float relWaterHeight = 1.0f;
        float relWaterWidth = 0.6f;
        float relWaterDepth = 1.0f;

        // dam break

        // compute number of particles

        float r = 0.3 * h;    // particle radius w.r.t. cell size
        float dx = 2.0 * r;
        float dy = sqrt(3.0) / 2.0 * dx;
        float dz = 2.0 * r;

        int numX = floor((relWaterWidth * tankWidth - 2.0 * h - 2.0 * r) / dx);
        int numY = floor((relWaterHeight * tankHeight - 2.0 * h - 2.0 * r) / dy);
        int numZ = floor((relWaterDepth * tankDepth - 2.0 * h - 2.0 * r) / dz);
        int maxParticles = numX * numY * numZ;

        // create fluid

        fluid = new FlipFluid(density, tankWidth, tankHeight, tankDepth, h, r, maxParticles);

        // create particles

        fluid->numParticles = numX * numY * numZ;
        int p = 0;
        for (int i = 0; i < numX; i++) {
            for (int j = 0; j < numY; j++) {
                for (int k = 0; k < numZ; k++) {
                    fluid->particlePos[p++] = vec3(
                            h + r + dx * i + (j % 2 == 0 ? 0.0 : r),
                            h + r + dy * j,
                            h + dz * k
                    );
                }
            }
        }

        // setup grid cells for tank

        int nx = fluid->fNumX;
        int ny = fluid->fNumY;
        int nz = fluid->fNumZ;

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    float s = 1.0;    // fluid
                    if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1)
                        s = 0.0;    // solid
                    fluid->s[i * ny * nz + j * nz + k] = s;
                }
            }
        }
    }

private:
};

PicFlip::PicFlip() : Simulation(){

}

PicFlip::~PicFlip(){

}

bool PicFlip::initialize() {
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

    setupScene();

    return true;
}

void PicFlip::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POS_ATTRIB);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(
            10.0f * Vec3<float>(0.0f, 0.0f, val * 10.0f - 6.0f) - Vec3<float>(15.f));
    Matrix4<float> translation2;
    for(int i = 0; i < fluid->numParticles; i++) {
        translation2 = translation2.Translation(
                10.0f * Vec3<float>(fluid->particlePos[i].x, fluid->particlePos[i].y, fluid->particlePos[i].z));
        Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
        Matrix4<float> MVP = perspective * translation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &MVP.cell);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.5f * fluid->particleVel[i].x + 0.5f,
                -0.5f * fluid->particleVel[i].y + 0.5f,
                -0.5f * fluid->particleVel[i].z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POS_ATTRIB);

    for(int i = 0; i < 2; i++){
        simulate(-accelerometerVector);
    }

    checkGlError("Renderer::render");
    framesRendered++;
}

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
        //wallpaper = new Box();
        //wallpaper = new Naive();
        wallpaper = new PicFlip();
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