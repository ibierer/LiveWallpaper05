//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_IMPLICITGRAPHER_H
#define LIVEWALLPAPER05_IMPLICITGRAPHER_H


#include <GLES3/gl32.h>
#include "PositionXYZNormalXYZ.h"

class ImplicitGrapher {
private:

    static const int maxNumOfEquations = 1024;

public:

    static string memoryEquations[maxNumOfEquations][2];

    static void processEquation(const int& i);

    static int numOfEquationsInMemory;

    static int surfaceEquation;

    explicit ImplicitGrapher(const ivec3& radius);

    ~ImplicitGrapher();

    static PositionXYZNormalXYZ* vertices;

    static uvec3* indices;

    static uint numIndices;

    static float fOfXYZ(vec3 position, const float& t, const vec3& offset, const float& zoom);

    static void calculateSurfaceOnCPU(float (*fOfXYZ)(vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, PositionXYZNormalXYZ* _vertices, uvec3* _indices, GLuint& _numIndices);

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);

private:

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

    static const int numOfDefaultEquations = 41;

    static const int numOfFunctions = 17;

    static const string defaultEquations[numOfDefaultEquations][2];

    static const string functions[numOfFunctions];

    static string debug_string;

    static double t;

    static bool* plusMinus;

    static bool* withinGraphRadius;

    static ivec3* xyzLineIndex;

    static ivec3* groupSegments;

    static int constant[maxNumOfEquations][maxEquationLength];

    static int valuesCounter[maxNumOfEquations];

    static float values[maxEquationLength];

    static float equationValues[maxNumOfEquations][maxEquationLength];

    static int sequences[maxNumOfEquations][maxEquationLength][3];

    static int sequenceLengths[maxNumOfEquations];

    static ivec3 radius;

    static ivec3 radiusPlusOne;

    static ivec3 size;

    static ivec3 sizePlus2;

    static ivec3 sizePlus3;

    static vec3 defaultOffset;

    static vec3 currentOffset;

    static int maxSolutionCount;

    static inline int node0(const int& i, const int& j, const int& k, const int& l);

    static inline int node1(const int& i, const int& j, const int& k, const int& l);

    static inline int node2(const int& i, const int& j, const int& k, const int& l);

    static inline int node3(const int& i, const int& j, const int& k, const int& l);

    static int solutionCount;

    static int groupSegmentCounter;

    static string decode(const int* const codedEquation, const int& length, const float* const values);

    static bool aDigit(const char& character);

    static bool aCharacter(const char& character);

    static string charToString(const char* const characters, const int& length);

    static void writeDefaultEquationsToMemory();

    static void writeUserEquationsToMemory();

    static bool getPlusMinus(const int& i, const int& j, const int& k);

    static inline ivec3 getXYZLineIndex(const int& i, const int& j, const int& k);

    static void refactor(const ivec3& inputRadius);

};


#endif //LIVEWALLPAPER05_IMPLICITGRAPHER_H
