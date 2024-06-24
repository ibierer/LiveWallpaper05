//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_IMPLICITGRAPHER_H
#define LIVEWALLPAPER05_IMPLICITGRAPHER_H

#include "PositionXYZNormalXYZ.h"
#include "NaiveSimulation.h"

class ImplicitGrapher {
private:

    static const int maxEquationLength = 4096;

public:

    struct chunk {
        bool plusMinus[32];
        ivec3 xyzLineIndex[32];
        PositionXYZNormalXYZ vertices[3 * 32];
        uvec3 indices[3 * 3 * 32];
        int solutionCount;
        int numIndices;
        bool padding[2008];
    };

    struct GPUdata {
        chunk chunks[1024];
        ivec3 sequence[maxEquationLength];
        int constants[maxEquationLength];
        float equationValues[maxEquationLength];
        int valuesCounter;
    };

    string memoryEquation;

    void processEquation();

    explicit ImplicitGrapher(const ivec3& size, PositionXYZNormalXYZ*& vertices, uvec3*& indices, const bool& vectorPointsPositive);

    ~ImplicitGrapher();

    static size_t getRecommendedIndicesArraySize();

    const std::function<float(vec3 position)> fOfXYZ = [this](vec3 position) {
        position -= currentOffset;
        position *= zoom;
        float* v = values;
        for (int i = 0; i < valuesCounter; i++) {
            switch (constants[i]) {
                case 0: v[i] = equationValues[i]; break;
                case X: v[i] = position.x; break;
                case Y: v[i] = position.y; break;
                case Z: v[i] = position.z; break;
                case T: v[i] = t; break;
                case E: v[i] = M_E; break;
                case PI: v[i] = M_PI; break;
            }
        }
        for (int i = 0; i < sequenceLength; i++) {
            int* s = sequences[i].v;
            switch (s[0]) {
                case ADD: v[s[1]] += v[s[2]]; break;
                case SUBTRACT: v[s[1]] -= v[s[2]]; break;
                case MULTIPLY: v[s[1]] *= v[s[2]]; break;
                case DIVIDE: v[s[1]] /= v[s[2]]; break;
                case POWER: v[s[1]] = powf(v[s[1]], v[s[2]]); break;
                case SINE: v[s[1]] = sinf(v[s[1]]); break;
                case COSINE: v[s[1]] = cosf(v[s[1]]); break;
                case TANGENT: v[s[1]] = tanf(v[s[1]]); break;
                case ARC_SINE: v[s[1]] = asinf(v[s[1]]); break;
                case ARC_COSINE: v[s[1]] = acosf(v[s[1]]); break;
                case ARC_TANGENT: v[s[1]] = atanf(v[s[1]]); break;
                case HYPERBOLIC_SINE: v[s[1]] = sinhf(v[s[1]]); break;
                case HYPERBOLIC_COSINE: v[s[1]] = coshf(v[s[1]]); break;
                case HYPERBOLIC_TANGENT: v[s[1]] = tanhf(v[s[1]]); break;
                case HYPERBOLIC_ARC_SINE: v[s[1]] = asinhf(v[s[1]]); break;
                case HYPERBOLIC_ARC_COSINE: v[s[1]] = acoshf(v[s[1]]); break;
                case HYPERBOLIC_ARC_TANGENT: v[s[1]] = atanhf(v[s[1]]); break;
                case ABSOLUTE_VALUE: v[s[1]] = abs(v[s[1]]); break;
                case LOG: v[s[1]] = log10f(v[s[1]]); break;
                case NATURAL_LOG: v[s[1]] = logf(v[s[1]]); break;
                case SQUARE_ROOT: v[s[1]] = sqrtf(v[s[1]]); break;
                case CUBED_ROOT: v[s[1]] = cbrtf(v[s[1]]); break;
            }
        }
        return v[0];
    };

    void calculateSurfaceOnCPU(std::function<float(vec3 _)> fOfXYZ, const float &timeVariable, const uint &iterations, const vec3 &offset, const float &zoom, const bool &clipEdges, PositionXYZNormalXYZ *_vertices, uvec3 *_indices, GLuint &_numIndices);

    ImplicitGrapher();

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);

    vec3 defaultOffset;

    int valuesCounter;

    static int hasTimeVariable;

    int sequenceLength;

    int constants[maxEquationLength];

    float values[maxEquationLength];

    float equationValues[maxEquationLength];

    ivec3 sequences[maxEquationLength];

    static void convertPiSymbol(string &basicString);

    static string checkEquationSyntax(const string& editable);

private:

    static const char pi = 5;

    static const int X = -1;

    static const int Y = -2;

    static const int Z = -3;

    static const int T = -4;

    static const int E = -5;

    static const int PI = -6;

    static const int OPEN_PARENTHESIS = -7;

    static const int CLOSE_PARENTHESIS = -8;

    static const int ADD = -9;

    static const int SUBTRACT = -10;

    static const int MULTIPLY = -11;

    static const int DIVIDE = -12;

    static const int POWER = -13;

    static const int SINE = -14;

    static const int COSINE = -15;

    static const int TANGENT = -16;

    static const int ARC_SINE = -17;

    static const int ARC_COSINE = -18;

    static const int ARC_TANGENT = -19;

    static const int HYPERBOLIC_SINE = -20;

    static const int HYPERBOLIC_COSINE = -21;

    static const int HYPERBOLIC_TANGENT = -22;

    static const int HYPERBOLIC_ARC_SINE = -23;

    static const int HYPERBOLIC_ARC_COSINE = -24;

    static const int HYPERBOLIC_ARC_TANGENT = -25;

    static const int ABSOLUTE_VALUE = -26;

    static const int LOG = -27;

    static const int NATURAL_LOG = -28;

    static const int SQUARE_ROOT = -29;

    static const int CUBED_ROOT = -30;

    static const int UNDERSCORE = -31;

    static const int POSITIVE = 1;

    static const int NEGATIVE = -1;

    static const int numOfFunctions = 17;

    static const string functions[numOfFunctions];

    static string debug_string;

    static double t;

    static bool* plusMinus;

    static bool* withinGraphRadius;

    static ivec3* xyzLineIndex;

    static ivec3* groupSegments;

    bool vectorPointsPositive;

    ivec3 size;

    ivec3 sizePlus2;

    ivec3 sizePlus3;

    vec3 currentOffset;

    static int maxSolutionCount;

    static int solutionCount;

    static int groupSegmentCounter;

    float zoom;

    void refactor(const ivec3& inputSize);

    inline int node0(const int& i, const int& j, const int& k, const int& l);

    inline int node1(const int& i, const int& j, const int& k, const int& l);

    inline int node2(const int& i, const int& j, const int& k, const int& l);

    inline int node3(const int& i, const int& j, const int& k, const int& l);

    static string decode(const int* const codedEquation, const int& length, const float* const values);

    static bool aDigit(const char& character);

    static bool aCharacter(const char& character);

    static string charToString(const char* const characters, const int& length);

    inline bool getPlusMinus(const int& i, const int& j, const int& k);

    inline ivec3 getXYZLineIndex(const int& i, const int& j, const int& k);

    static bool anOperator(const char& character);

};

#endif //LIVEWALLPAPER05_IMPLICITGRAPHER_H
