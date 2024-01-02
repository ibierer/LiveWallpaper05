//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_IMPLICITGRAPHER_H
#define LIVEWALLPAPER05_IMPLICITGRAPHER_H


#include <GLES3/gl32.h>
#include "PositionXYZNormalXYZ.h"

class ImplicitGrapher {

    static const char pi = 5;

    static const int NAME = 0;

    static const int EQUATION = 1;

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

    static const int maxEquationLength = 4096;

    static const int maxNumOfEquations = 1024;

    static const int numOfDefaultEquations = 41;

    static const int numOfFunctions = 17;

    static const string defaultEquations[numOfDefaultEquations][2];

    static const string functions[numOfFunctions];

    string debug_string;

    double t;

    bool* plusMinus;

    bool* withinGraphRadius;

    ivec3* xyzLineIndex;

    ivec3* groupSegments;

    int constant[maxNumOfEquations][maxEquationLength];

    int valuesCounter[maxNumOfEquations];

    float values[maxEquationLength];

    float equationValues[maxNumOfEquations][maxEquationLength];

    int sequences[maxNumOfEquations][maxEquationLength][3];

    int sequenceLengths[maxNumOfEquations];

    ivec3 radius;

    ivec3 radiusPlusOne;

    ivec3 size;

    ivec3 sizePlus2;

    ivec3 sizePlus3;

    vec3 defaultOffset;

    vec3 currentOffset;

    int maxSolutionCount;

    inline int node0(const int& i, const int& j, const int& k, const int& l);

    inline int node1(const int& i, const int& j, const int& k, const int& l);

    inline int node2(const int& i, const int& j, const int& k, const int& l);

    inline int node3(const int& i, const int& j, const int& k, const int& l);

    int solutionCount;

    int groupSegmentCounter;

    string decode(const int* const codedEquation, const int& length, const float* const values);

    static bool aDigit(const char& character);

    static bool aCharacter(const char& character);

    static string charToString(const char* const characters, const int& length);

    void writeDefaultEquationsToMemory();

    void writeUserEquationsToMemory();

    bool getPlusMinus(const int& i, const int& j, const int& k);

    inline ivec3 getXYZLineIndex(const int& i, const int& j, const int& k);

    void refactor(const ivec3& inputRadius);

public:

    string memoryEquations[maxNumOfEquations][2];

    void processEquation(const int& i);

    int numOfEquationsInMemory;

    int surfaceEquation;

    explicit ImplicitGrapher(const ivec3& radius);

    ~ImplicitGrapher();

    PositionXYZNormalXYZ* vertices;

    uvec3* indices;

    uint numIndices;

    static float fOfXYZ(ImplicitGrapher& graph, vec3 _, const float& t, const vec3& offset, const float& zoom);

    void calculateSurfaceOnCPU(float (*fOfXYZ)(ImplicitGrapher&, vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, PositionXYZNormalXYZ* _vertices, uvec3* _indices, GLuint& _numIndices);

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);
};


#endif //LIVEWALLPAPER05_IMPLICITGRAPHER_H
