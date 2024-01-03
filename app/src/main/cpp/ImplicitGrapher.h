//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_IMPLICITGRAPHER_H
#define LIVEWALLPAPER05_IMPLICITGRAPHER_H


#include "PositionXYZNormalXYZ.h"

class ImplicitGrapher {
private:

    static const int maxNumOfEquations = 1024;

    static const int maxEquationLength = 4096;

public:

    GLuint generateComputeShader(const char* computeShaderSrcCode) {
        // Create the compute program, to which the compute shader will be assigned
        GLuint gComputeProgram = glCreateProgram();
        // Create and compile the compute shader
        GLuint mComputeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(mComputeShader, 1, &computeShaderSrcCode, NULL);
        glCompileShader(mComputeShader);
        // Check if there were any issues when compiling the shader
        int rvalue;
        glGetShaderiv(mComputeShader, GL_COMPILE_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetShaderiv(mComputeShader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetShaderInfoLog(mComputeShader, infoLen, NULL, log);
                ALOGE("Could not compile compute shader:\n%s\n", log);
                free(log);
            }
            glDeleteShader(mComputeShader);
            return 0;
        }
        // Attach and link the shader against to the compute program
        glAttachShader(gComputeProgram, mComputeShader);
        glLinkProgram(gComputeProgram);
        // Check if there were some issues when linking the shader.
        glGetProgramiv(gComputeProgram, GL_LINK_STATUS, &rvalue);
        if (!rvalue) {
            GLint infoLen = 0;
            glGetProgramiv(gComputeProgram, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* log = (char*)malloc(sizeof(char) * infoLen);
                glGetShaderInfoLog(mComputeShader, infoLen, NULL, log);
                ALOGE("Error linking program:\n%s\n", log);
                free(log);
            }
            glDeleteProgram(gComputeProgram);
            return 0;
        }
        return gComputeProgram;
    }

    static GLuint computeShaderProgram;

    static GLuint computeShaderVBO;

    struct chunk {
        bool plusMinus[32];
        ivec3 xyzLineIndex[32];
        ivec3 groupSegments[3 * 32];
        bool withinGraphRadius[3 * 32];
        PositionXYZNormalXYZ vertices[3 * 32];
        uvec3 indices[3 * 3 * 32];
        int solutionCount;
        int numIndices;
        int groupSegmentCounter;
        bool padding[756];
    };

    struct data {
        chunk chunks[1024];
        int sequence[maxEquationLength][3];
        int constants[maxEquationLength];
        float equationValues[maxEquationLength];
        int valuesCounter;
    };

    const string computeShaderCode[1000] = {
            View::ES_VERSION,
            "const int X = -1;\n",
            "const int Y = -2;\n",
            "const int Z = -3;\n",
            "const int T = -4;\n",
            "const int E = -5;\n",
            "const int PI = -6;\n",
            "const int OPEN_PARENTHESIS = -7;\n",
            "const int CLOSE_PARENTHESIS = -8;\n",
            "const int ADD = -9;\n",
            "const int SUBTRACT = -10;\n",
            "const int MULTIPLY = -11;\n",
            "const int DIVIDE = -12;\n",
            "const int POWER = -13;\n",
            "const int SINE = -14;\n",
            "const int COSINE = -15;\n",
            "const int TANGENT = -16;\n",
            "const int ARC_SINE = -17;\n",
            "const int ARC_COSINE = -18;\n",
            "const int ARC_TANGENT = -19;\n",
            "const int HYPERBOLIC_SINE = -20;\n",
            "const int HYPERBOLIC_COSINE = -21;\n",
            "const int HYPERBOLIC_TANGENT = -22;\n",
            "const int HYPERBOLIC_ARC_SINE = -23;\n",
            "const int HYPERBOLIC_ARC_COSINE = -24;\n",
            "const int HYPERBOLIC_ARC_TANGENT = -25;\n",
            "const int ABSOLUTE_VALUE = -26;\n",
            "const int LOG = -27;\n",
            "const int NATURAL_LOG = -28;\n",
            "const int SQUARE_ROOT = -29;\n",
            "const int CUBED_ROOT = -30;\n",
            "const int UNDERSCORE = -31;\n",
            "const int POSITIVE = 1;\n",
            "const int NEGATIVE = -1;\n",
            "struct PositionXYZNormalXYZ {",
            "    vec3 p;\n",
            "    vec3 n;\n",
            "};\n",
            "struct chunk {\n",
            "    bool plusMinus[32];\n",
            "    ivec3 xyzLineIndex[32];\n",
            "    ivec3 groupSegments[3 * 32];\n",
            "    bool withinGraphRadius[3 * 32];\n",
            "    PositionXYZNormalXYZ vertices[3 * 32];\n",
            "    uvec3 indices[3 * 3 * 32];\n",
            "    int solutionCount;\n",
            "    int numIndices;\n",
            "    int groupSegmentCounter;\n",
            "    bool padding[756];\n",
            "};\n",
            "layout(packed, binding = 0) buffer destBuffer {\n",
            "    chunk chunks[1024];\n",
            "    int sequence[4096][3];\n",
            "    int constants[4096];\n",
            "    float equationValues[4096];\n",
            "    int valuesCounter;\n",
            "} outBuffer;\n",
            "uniform vec3 currentOffset;\n",
            "uniform float t;\n",
            "uniform float zoom;\n",
            "uniform int valuesCounter;\n",
            "uniform int sequenceLength;\n",
            "uniform int surfaceEquation;\n",
            "uniform int iterations;\n",
            "const ivec3 sizePlus3 = ivec3(32);\n",
            "float values[4096];\n",
            "float fOfXYZ(vec3 position) {\n",
            "    position -= currentOffset;\n",
            "    position *= zoom;\n",
            "    for (int i = 0; i < valuesCounter; i++) {\n",
            "        switch (outBuffer.constants[i]) {\n",
            "            case 0: values[i] = outBuffer.equationValues[i]; break;\n",
            "            case X: values[i] = position.x; break;\n",
            "            case Y: values[i] = position.y; break;\n",
            "            case Z: values[i] = position.z; break;\n",
            "            case T: values[i] = t; break;\n",
            "            case E: values[i] = 2.718281828459045f; break;\n",
            "            case PI: values[i] = 3.141592653589793f; break;\n",
            "        }\n",
            "    }\n",
            "    for (int i = 0; i < sequenceLength; i++) {\n",
            "        switch (outBuffer.sequence[i][0]) {\n",
            "            case ADD: values[outBuffer.sequence[i][1]] += values[outBuffer.sequence[i][2]]; break;\n",
            "            case SUBTRACT: values[outBuffer.sequence[i][1]] -= values[outBuffer.sequence[i][2]]; break;\n",
            "            case MULTIPLY: values[outBuffer.sequence[i][1]] *= values[outBuffer.sequence[i][2]]; break;\n",
            "            case DIVIDE: values[outBuffer.sequence[i][1]] /= values[outBuffer.sequence[i][2]]; break;\n",
            "            case POWER: values[outBuffer.sequence[i][1]] = pow(values[outBuffer.sequence[i][1]], values[outBuffer.sequence[i][2]]); break;\n",
            "            case SINE: values[outBuffer.sequence[i][1]] = sin(values[outBuffer.sequence[i][1]]); break;\n",
            "            case COSINE: values[outBuffer.sequence[i][1]] = cos(values[outBuffer.sequence[i][1]]); break;\n",
            "            case TANGENT: values[outBuffer.sequence[i][1]] = tan(values[outBuffer.sequence[i][1]]); break;\n",
            "            case ARC_SINE: values[outBuffer.sequence[i][1]] = asin(values[outBuffer.sequence[i][1]]); break;\n",
            "            case ARC_COSINE: values[outBuffer.sequence[i][1]] = acos(values[outBuffer.sequence[i][1]]); break;\n",
            "            case ARC_TANGENT: values[outBuffer.sequence[i][1]] = atan(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_SINE: values[outBuffer.sequence[i][1]] = sinh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_COSINE: values[outBuffer.sequence[i][1]] = cosh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_TANGENT: values[outBuffer.sequence[i][1]] = tanh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_ARC_SINE: values[outBuffer.sequence[i][1]] = asinh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_ARC_COSINE: values[outBuffer.sequence[i][1]] = acosh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case HYPERBOLIC_ARC_TANGENT: values[outBuffer.sequence[i][1]] = atanh(values[outBuffer.sequence[i][1]]); break;\n",
            "            case ABSOLUTE_VALUE: values[outBuffer.sequence[i][1]] = abs(values[outBuffer.sequence[i][1]]); break;\n",
            "            case LOG: values[outBuffer.sequence[i][1]] = log(values[outBuffer.sequence[i][1]]) / log(10.0f); break;\n",
            "            case NATURAL_LOG: values[outBuffer.sequence[i][1]] = log(values[outBuffer.sequence[i][1]]); break;\n",
            "            case SQUARE_ROOT: values[outBuffer.sequence[i][1]] = sqrt(values[outBuffer.sequence[i][1]]); break;\n",
            "            case CUBED_ROOT: values[outBuffer.sequence[i][1]] = pow(values[outBuffer.sequence[i][1]], 1.0f / 3.0f); break;\n",
            "        }\n",
            "    }\n",
            "    return values[0];\n",
            "}\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "void main() {\n",
            "    uint task = gl_WorkGroupSize.x * gl_LocalInvocationID.y + gl_LocalInvocationID.y;\n",
            "    // Erase normals\n",
            "    for (int i = 0; i < outBuffer.chunks[task].solutionCount; i++) {\n",
            "        outBuffer.chunks[task].vertices[i].n = vec3(0.0f);\n",
            "    }\n",
            "    // Reset solution count\n",
            "    outBuffer.chunks[task].solutionCount = 0;\n",
            "    // Reset group segment counter\n",
            "    outBuffer.chunks[task].groupSegmentCounter = 0;\n",
            "    // Calculate plusMinusAmounts and approximate solutions\n",
            "    for (int k = 0; k < sizePlus3.z; k++) {\n",
            "        outBuffer.chunks[task].plusMinus[k] = fOfXYZ(vec3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k)) > 0.0f;\n",
            "        uvec3 ijk = uvec3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k);\n",
            "        for (int l = 0; l < 3; l++) {\n",
            "            if (ijk[l] == 0u) {\n",
            "                continue;\n",
            "            }\n",
            "            uvec3 iCursor = ijk;\n",
            "            iCursor[l] -= 1u;\n",
            "            bool firstSample = outBuffer.chunks[task].plusMinus[iCursor.z];\n",
            "            bool secondSample = outBuffer.chunks[task].plusMinus[ijk.z];\n",
            "            // If no solution is detected jump to next iteration of loop.\n",
            "            if (firstSample == secondSample) {\n",
            "                continue;\n",
            "            }\n",
            "            outBuffer.chunks[task].xyzLineIndex[iCursor.z][l] = outBuffer.chunks[task].solutionCount;\n",
            "            int sign = secondSample ? POSITIVE : NEGATIVE;\n",
            "            for (int m = 0; m < 3; m++) {\n",
            "                outBuffer.chunks[task].vertices[outBuffer.chunks[task].solutionCount].p[m] = float(iCursor[m]);\n",
            "            }\n",
            "            float scan = 0.5f;\n",
            "            for (int m = 0; m < iterations; m++) {// Maybe use a do-while loop here to reduce the scan operations by 1.\n",
            "                _vertices[solutionCount].p[l] += scan;\n",
            "                if (sign == POSITIVE ^ fOfXYZ(_vertices[solutionCount].p) > 0.0f) {\n",
            "                    scan *= 0.5f;\n",
            "                }else{\n",
            "                    sign *= -1;\n",
            "                    scan *= -0.5f;\n",
            "                }\n",
            "            }\n",
            "            solutionCount++;\n",
            "        }\n",
            "        if (i == 0 || j == 0 || k == 0) {\n",
            "            continue;\n",
            "        }\n",
            "        int groupSegmentStartIndex = groupSegmentCounter;\n",
            "        bool xyz = getPlusMinus(i - 1, j - 1, k - 1);\n",
            "        bool xyZ = getPlusMinus(i - 1, j - 1, k);\n",
            "        bool xYz = getPlusMinus(i - 1, j, k - 1);\n",
            "        bool xYZ = getPlusMinus(i - 1, j, k);\n",
            "        bool Xyz = getPlusMinus(i, j - 1, k - 1);\n",
            "        bool XyZ = getPlusMinus(i, j - 1, k);\n",
            "        bool XYz = getPlusMinus(i, j, k - 1);\n",
            "        bool XYZ = getPlusMinus(i, j, k);\n",
            "        for (int l = 0; l < 6; l++) {\n",
            "            int combo = 0;\n",
            "            switch (l) {\n",
            "                case 0://XY\n",
            "                    combo = xYz * 8 | XYz * 4 | xyz * 2 | Xyz * 1;\n",
            "                    break;\n",
            "                case 1://XY\n",
            "                    combo = xYZ * 8 | XYZ * 4 | xyZ * 2 | XyZ * 1;\n",
            "                    break;\n",
            "                case 2://YZ\n",
            "                    combo = xyZ * 8 | xYZ * 4 | xyz * 2 | xYz * 1;\n",
            "                    break;\n",
            "                case 3://YZ\n",
            "                    combo = XyZ * 8 | XYZ * 4 | Xyz * 2 | XYz * 1;\n",
            "                    break;\n",
            "                case 4://ZX\n",
            "                    combo = Xyz * 8 | XyZ * 4 | xyz * 2 | xyZ * 1;\n",
            "                    break;\n",
            "                case 5://ZX\n",
            "                    combo = XYz * 8 | XYZ * 4 | xYz * 2 | xYZ * 1;\n",
            "                    break;\n",
            "            }\n",
            "            switch (combo) {\n",
            "                case 0: break;//0000\n",
            "                case 1://0001\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 2://0010\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 3://0011\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1); break;\n",
            "                case 4://0100\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 5://0101\n",
            "                    if(l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 6: {//0110\n",
            "                    int _node0 = node0(i, j, k, l);\n",
            "                    int _node1 = node1(i, j, k, l);\n",
            "                    int _node2 = node2(i, j, k, l);\n",
            "                    int _node3 = node3(i, j, k, l);\n",
            "                    if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f) > 0.0f) {\n",
            "                        if (l % 2 != vectorPointsPositive){\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);\n",
            "                        }\n",
            "                    } else {\n",
            "                        if (l % 2 != vectorPointsPositive) {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);\n",
            "                        }\n",
            "                    }\n",
            "                    break;\n",
            "                }\n",
            "                case 7://0111\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 8://1000\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);\n",
            "                    break;\n",
            "                case 9: {//1001\n",
            "                    int _node0 = node0(i, j, k, l);\n",
            "                    int _node1 = node1(i, j, k, l);\n",
            "                    int _node2 = node2(i, j, k, l);\n",
            "                    int _node3 = node3(i, j, k, l);\n",
            "                    if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f) > 0.0f) {\n",
            "                        if (l % 2 != vectorPointsPositive) {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);\n",
            "                        }\n",
            "                    } else {\n",
            "                        if (l % 2 != vectorPointsPositive) {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);\n",
            "                        }\n",
            "                    }\n",
            "                    break;\n",
            "                }\n",
            "                case 10://1010\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1); break;\n",
            "                case 11://1011\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1); break;\n",
            "                case 12://1100\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1); break;\n",
            "                case 13://1101\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1); break;\n",
            "                case 14://1110\n",
            "                    if (l % 2 != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1); break;\n",
            "                case 15:\n",
            "                    break;//1111\n",
            "            }\n",
            "        }\n",
            "        //Grouping\n",
            "        //{73}{64}{0}    {73}{64}{0}    {73}{64}{107}\n",
            "        //{97}{21}{0}    {64}{89}{0}    {64}{89}{107}\n",
            "        //{89}{64}{0}    {89}{80}{0}    {89}{80}{107}\n",
            "        //{89}{80}{0}    {80}{73}{0}    {80}{73}{107}\n",
            "        //{54}{21}{0}    {21}{97}{0}    {21}{97}{54}\n",
            "        //{54}{97}{0}    {97}{54}{0}    {97}{54}{0}\n",
            "        //{80}{73}{0}    {54}{21}{0}    {54}{21}{0}\n",
            "        int segPerGroup = 1;\n",
            "        if (clipEdges) {\n",
            "            for (int l = groupSegmentStartIndex; l < groupSegmentCounter; l++) {\n",
            "                withinGraphRadius[l] = i > 1 && j > 1 && k > 1 && i < sizePlus2.x && j < sizePlus2.y && k < sizePlus2.z;\n",
            "            }\n",
            "        }\n",
            "        for (int l = groupSegmentStartIndex; l < groupSegmentCounter - 1; l++) {\n",
            "            for (int _m = l + 1; _m < groupSegmentCounter; _m++) {\n",
            "                if (groupSegments[_m][0] != groupSegments[l][1]) {\n",
            "                    continue;\n",
            "                }\n",
            "                //Swap\n",
            "                int storeA = groupSegments[l + 1][0];\n",
            "                int storeB = groupSegments[l + 1][1];\n",
            "                groupSegments[l + 1][0] = groupSegments[_m][0];\n",
            "                groupSegments[l + 1][1] = groupSegments[_m][1];\n",
            "                groupSegments[_m][0] = storeA;\n",
            "                groupSegments[_m][1] = storeB;\n",
            "                segPerGroup++;\n",
            "                //Check to see if end of loop\n",
            "                if (groupSegments[l + 1][1] != groupSegments[l + 2 - segPerGroup][0]) {\n",
            "                    continue;\n",
            "                }\n",
            "                if (segPerGroup == 3) {\n",
            "                    groupSegments[l - 1][2] = groupSegments[l][1];\n",
            "                }\n",
            "                else {\n",
            "                    vec3 sum(0.0f);\n",
            "                    for (int _n = l - segPerGroup + 2; _n < l + 2; _n++) {\n",
            "                        sum += _vertices[groupSegments[_n][0]].p;\n",
            "                        groupSegments[_n][2] = solutionCount;\n",
            "                    }\n",
            "                    _vertices[solutionCount].p = sum / segPerGroup;\n",
            "                    solutionCount++;\n",
            "                }\n",
            "                segPerGroup = 1;\n",
            "                l++;\n",
            "                break;\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    // Calculate face normals and add them to the per-solution normals.\n",
            "    for (int i = 0; i < groupSegmentCounter; i++) {\n",
            "        if (groupSegments[i][2] > -1) {\n",
            "            vec3 vectorA = _vertices[groupSegments[i][0]].p - _vertices[groupSegments[i][2]].p;\n",
            "            vec3 vectorB = _vertices[groupSegments[i][1]].p - _vertices[groupSegments[i][2]].p;\n",
            "            vec3 crossProduct = cross(vectorA, vectorB);\n",
            "            float length = distance(crossProduct);\n",
            "            vec3 normalizedCrossProduct = crossProduct / length;\n",
            "            _vertices[groupSegments[i][0]].n += normalizedCrossProduct;\n",
            "            _vertices[groupSegments[i][1]].n += normalizedCrossProduct;\n",
            "            _vertices[groupSegments[i][2]].n += normalizedCrossProduct;\n",
            "        }\n",
            "    }\n",
            "    //Normalize\n",
            "    for (int i = 0; i < solutionCount; i++) {\n",
            "        float length = (vectorPointsPositive) ? -distance(vec3(_vertices[i].n)) : distance(vec3(_vertices[i].n));\n",
            "        _vertices[i].n /= length;\n",
            "    }\n",
            "    //Create TriangleView Primitives\n",
            "    _numIndices = 0;\n",
            "    int triangleCount = 0;\n",
            "    for (int i = 0; i < groupSegmentCounter; i++) {\n",
            "        if (groupSegments[i][2] > -1 && (!clipEdges || withinGraphRadius[i])) {\n",
            "            _indices[triangleCount++] = groupSegments[i];\n",
            "            _numIndices += 3;\n",
            "        }\n",
            "    }\n",
            "    for (int i = 0; i < solutionCount; i++) {\n",
            "        _vertices[i].p -= currentOffset;\n",
            "    }\n",
            "}"};

    static string memoryEquations[maxNumOfEquations][2];

    static void processEquation(const int& i);

    static int numOfEquationsInMemory;

    static int surfaceEquation;

    explicit ImplicitGrapher(const ivec3& radius);

    ~ImplicitGrapher();

    static PositionXYZNormalXYZ* vertices;

    static uvec3* indices;

    static uint numIndices;

    static float fOfXYZ(vec3 position);

    static void calculateSurfaceOnCPU(float (*fOfXYZ)(vec3),
                                      const float &timeVariable, const uint &iterations,
                                      const vec3 &offset,
                                      const float &zoom, const bool &vectorPointsPositive,
                                      const bool &clipEdges, PositionXYZNormalXYZ *_vertices,
                                      uvec3 *_indices,
                                      GLuint &_numIndices);

    static void calculateSurfaceOnGPU(float (*fOfXYZ)(vec3),
                                      const float &timeVariable, const uint &iterations,
                                      const vec3 &offset,
                                      const float &zoom, const bool &vectorPointsPositive,
                                      const bool &clipEdges, PositionXYZNormalXYZ *_vertices,
                                      uvec3 *_indices,
                                      GLuint &_numIndices);

    ImplicitGrapher();

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);

    static vec3 defaultOffset;

    static vec3 offset;

    static uint iterations;

    static int valuesCounter[maxNumOfEquations];

    static int sequenceLengths[maxNumOfEquations];

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

    static const int POSITIVE = 1;

    static const int NEGATIVE = -1;

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

    static int constants[maxNumOfEquations][maxEquationLength];

    static float values[maxEquationLength];

    static float equationValues[maxNumOfEquations][maxEquationLength];

    static int sequences[maxNumOfEquations][maxEquationLength][3];

    static ivec3 radius;

    static ivec3 radiusPlusOne;

    static ivec3 size;

    static ivec3 sizePlus2;

    static ivec3 sizePlus3;

    static vec3 currentOffset;

    static int maxSolutionCount;

    static int solutionCount;

    static int groupSegmentCounter;

    static float zoom;

    static void refactor(const ivec3& inputRadius);

    static inline int node0(const int& i, const int& j, const int& k, const int& l);

    static inline int node1(const int& i, const int& j, const int& k, const int& l);

    static inline int node2(const int& i, const int& j, const int& k, const int& l);

    static inline int node3(const int& i, const int& j, const int& k, const int& l);

    static string decode(const int* const codedEquation, const int& length, const float* const values);

    static bool aDigit(const char& character);

    static bool aCharacter(const char& character);

    static string charToString(const char* const characters, const int& length);

    static void writeDefaultEquationsToMemory();

    static void writeUserEquationsToMemory();

    static bool getPlusMinus(const int& i, const int& j, const int& k);

    static inline ivec3 getXYZLineIndex(const int& i, const int& j, const int& k);

};


#endif //LIVEWALLPAPER05_IMPLICITGRAPHER_H
