//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_IMPLICITGRAPHER_H
#define LIVEWALLPAPER05_IMPLICITGRAPHER_H


#include <GLES3/gl32.h>
#include "VertexNormal.h"

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

    const string defaultEquations[numOfDefaultEquations][2] = {
            {"Two Toroids",                  "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Three Toroids",                "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2/3))^2 + (z + cos(t + 2/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4/3))^2 + (z + cos(t + 4/3))^2) = 5"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                },
            {"Four Toroids",                 "1/((sqrt(x^2 + y^2) - 1.6 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + /2))^2 + (z + cos(t + /2))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + ))^2 + (z + cos(t + ))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + 3/2))^2 + (z + cos(t + 3/2))^2) = 7.3"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         },
            {"Expanding Concentric Spheres", "sin(sqrt(x^2 + y^2 + z^2) - t) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Heart",                        "x^2 + ((y - (x^2)^(1/3.0)))^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Heart II",                     "(0.5x)^2 + (abs(0.5x) - 0.5y)^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Spiral",                       "sqrt((x - sin(z - t))^2 + (y - cos(z - t))^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Ripples",                      "3cos(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t)/2)/2)/2)/2)/2)/2)/2)/2)/2)/2)/sqrt(x^2 + y^2 + z^2) = z"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
            {"Circle",                       "x^2 + y^2 = 4"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Square",                       "-(0.5x)^100 + 1 = 0.5abs(y)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Sphere",                       "x^2 + y^2 + z^2 = 4"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Cylinder",                     "(0.5x)^2 + (0.5y)^40 + (0.5z)^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Orbit",                        ".2/((x + 2.5sin(t))^2 + (y + 2.5cos(t))^2 + z^2) + 1/((x - 0.5sin(t))^2 + (y - 0.5cos(t))^2 + z^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
            {"Arch",                         "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z = 1",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           },
            {"Dunking Doughnut",             "3/sqrt((sqrt(x^2 + (z - 6sin(t))^2) - 3)^2 + y^2) - z = 3",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Ring around Arch",             "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z + 1/sqrt((sqrt(y^2 + (z - 3)^2) - 1.5)^2 + x^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Chain",                        "1/sqrt((sqrt((x + 4.1 + sin(t)/3)^2 + y^2) - 3)^2 + z^2) + 1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) + 1/sqrt((sqrt((x - 4.1 - sin(t)/3)^2 + y^2) - 3)^2 + z^2) = 3"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Smiley Face",                  "(x^4 + y^4 + 2x^2y^2 - 2x^2 - 2y^2 + 1.3*2^y)((x^2 + y^2 - y + .6)^2 - 2x^2)(x^2 + y^2 - 4) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Pythagorean's Theorem",        "x^2 + y^2 = z^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Two Cubes",                    "1/(x^40 + y^40 + z^40) + 1/((x + 4sin(t))^40 + (y + 4cos(t))^40 + z^40) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Hole Thru Cube",               "1/((abs(0.3x) + 0.25)^20 + (abs(0.3y) + 0.25)^20 + (abs(0.3z) + 0.25)^20) - 1/((x)^2 + (y)^2)^100 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
            {"Guitar",                       "1/(x^2 + (.75(y + 1.2))^4 + (3z)^40) - .025/((x - .7)^2 + (y + .9)^2) - .025/((x + .7)^2 + (y + .9)^2) - .06/((x - .9)^2 + (y + .85)^2) - .06/((x + .9)^2 + (y + .85)^2) - 1/((4x)^2 + (4(y + .6))^2 + (10(z - .25))^10)^10 - 1/((2x)^2 + (2(y + .6))^2 + (5z)^40)^10 - 1/((1.5x)^2 + (1.4(y + 1.6))^2 + (5z)^40)^10 + 1/((4x)^2 + (y - 1)^40 + (10(z - .255))^2)^10 + 1/((4x/(y - 1))^40 + (3(y - 2.2))^40 + (10(z + .2y - .6))^10)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Skateboard",                   "1/(80(x + 2)^2 + 80y^2 + z^10)^10 + 1/(80(x - 2)^2 + 80y^2 + z^10)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(10(x - 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(10(x + 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(0.00002x^10 + 10000000000(y - 0.5 - (0.25x)^4)^10 + z^2)^10 = 1"                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 1",              "sqrt(sin(x)^2 + sin(y)^2 + sin(z)^2) - 0.5sin(t) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
            {"Miscellaneous 2",              "sin(x) + sin(y) + sin(z) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
            {"Miscellaneous 3",              "sin(x^2 + y^2) = cos(xy)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 4",              "sqrt(x^2 + y^2) = 1/sqrt((sqrt(x^2 + y^2))^2 + z^2)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Miscellaneous 5",              "abs(y) = 1/sqrt(x^2 + y^2)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Miscellaneous 6",              "abs(z) = 1/(x^2 + y^2) - 0.12"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Miscellaneous 7",              "x^3 + 5x^2 + xy^2 - 5y^2 = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
            {"Miscellaneous 8",              "xyz = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
            {"Miscellaneous 9",              "x + y + z = xyz"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Miscellaneous 10",             "(x/3)^40 + (y/3)^40 + (z/1.5)^40 + (((x - y)/6)^2 + ((y - x)/6)^2)^20 + (((-x - y)/6)^2 + ((x + y)/6)^2)^20 = 0.99"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 11",             "sqrt(x^2+y^2) = sin(t) + 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Miscellaneous 12",             "1/((0.5x)^2 + (0.5y)^2 + (0.5z)^2) - 1/(((0.5x) + sin(t))^2 + (0.5y + cos(t))^2 + (0.5z)^2)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 13",             "1/(x^2 + y^2) + 1/((x - sin(z - t))^2 + (y + cos(z - t))^2)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 14",             "x^2 + y^2 = 3atan(y/x)^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 15",             "x^2 + ((y^2)sin(y)) + ((z^2)cos(z)) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
            {"Miscellaneous 16",             "(x^2)sin(x)+ ((y^2)cos(y))+ ((z^2)sin(z)) = sin(t)^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
            {"Miscellaneous 17",             "(x + zsin(z + t))^2 + (y + zcos(z + t))^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Miscellaneous 18",             "1/(x^2 + ((y^2)sin(y)) + z^2) + 0.89/((x - 3sin(t))^2+(y - 3cos(t))^2 + z^40) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            //{"Flower",                       ""                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
    };

    const string functions[numOfFunctions] = {
            "sin(__",
            "cos(__",
            "tan(__",
            "asin(_",
            "acos(_",
            "atan(_",
            "sinh(_",
            "cosh(_",
            "tanh(_",
            "asinh(",
            "acosh(",
            "atanh(",
            "abs(__",
            "log(__",
            "ln(___",
            "sqrt(_",
            "cbrt(_",
    };

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

    VertexNormal* vertices;

    uvec3* indices;

    uint numIndices;

    static float fOfXYZ(ImplicitGrapher& graph, vec3 _, const float& t, const vec3& offset, const float& zoom);

    void calculateSurfaceOnCPU(float (*fOfXYZ)(ImplicitGrapher&, vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, VertexNormal* _vertices, uvec3* _indices, GLuint& _numIndices);

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);
};


#endif //LIVEWALLPAPER05_IMPLICITGRAPHER_H
