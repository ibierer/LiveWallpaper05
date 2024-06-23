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

    GLuint computeShaderProgram;

    GLuint computeShaderVBO;

    GLuint indexBufferBinding;

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

    //static GPUdata* data;

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
            "    PositionXYZNormalXYZ vertices[3 * 32];\n",
            "    uvec3 indices[3 * 3 * 32];\n",
            "    int solutionCount;\n",
            "    int numIndices;\n",
            "    bool padding[2008];\n",
            "};\n",
            "layout(packed, binding = 0) buffer destBuffer {\n",
            "    chunk chunks[1024];\n",
            "    ivec3 sequence[4096];\n",
            "    int constants[4096];\n",
            "    float equationValues[4096];\n",
            "    int valuesCounter;\n",
            "} outBuffer;\n",
            "uniform vec3 currentOffset;\n",
            "uniform float t;\n",
            "uniform float distanceToOrigin;\n",
            "uniform int valuesCounter;\n",
            "uniform int sequenceLength;\n",
            "uniform int surfaceEquation;\n",
            "uniform int iterations;\n",
            "uniform int vectorPointsPositive;\n",
            "uniform int clipEdges;\n",
            "const uvec3 sizePlus3 = uvec3(32u);\n",
            "const uvec3 sizePlus2 = uvec3(31u);\n",
            "float values[4096];\n",
            "ivec3 groupSegments[3 * 32];\n",
            "bool withinGraphRadius[3 * 32];\n",
            "uint task;\n",
            "layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;\n",
            "float dist(vec3 coordinates) {\n",
            "    return sqrt(dot(coordinates, coordinates));\n",
            "}\n",
            "int getPlusMinus(uint type, uint j, uint k){\n",
            "    return int(outBuffer.chunks[gl_WorkGroupSize.x * j + type].plusMinus[k]);\n",
            "}\n",
            "ivec3 getXYZLineIndex(uint type, uint j, uint k){\n",
            "    return outBuffer.chunks[gl_WorkGroupSize.x * j + type].xyzLineIndex[k];\n",
            "}\n",
            "int node0(uint type, uint j, uint k, uint l) {\n",
            "    switch (l) {\n",
            "        case 0: return getXYZLineIndex(type - 1u, j, k - 1u)[0]; break;//XY-plane[Yz]\n",
            "        case 1: return getXYZLineIndex(type - 1u, j, k)[0]; break;//XY-plane[YZ]\n",
            "        case 2: return getXYZLineIndex(type - 1u, j - 1u, k)[1]; break;//YZ-plane[xZ]\n",
            "        case 3: return getXYZLineIndex(type, j - 1u, k)[1]; break;//YZ-plane[XZ]\n",
            "        case 4: return getXYZLineIndex(type, j - 1u, k - 1u)[2]; break;//ZX-plane[Xy]\n",
            "        case 5: return getXYZLineIndex(type, j, k - 1u)[2]; break;//ZX-plane[XY]\n",
            "        default: return 0;\n",
            "    }\n",
            "}\n",
            "int node1(uint type, uint j, uint k, uint l) {\n",
            "    switch (l) {\n",
            "        case 0: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[1]; break;//XY-plane[xz]\n",
            "        case 1: return getXYZLineIndex(type - 1u, j - 1u, k)[1]; break;//XY-plane[xZ]\n",
            "        case 2: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[2]; break;//YZ-plane[xy]\n",
            "        case 3: return getXYZLineIndex(type, j - 1u, k - 1u)[2]; break;//YZ-plane[Xy]\n",
            "        case 4: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[0]; break;//ZX-plane[yz]\n",
            "        case 5: return getXYZLineIndex(type - 1u, j, k - 1u)[0]; break;//ZX-plane[Yz]\n",
            "        default: return 0;\n",
            "    }\n",
            "}\n",
            "int node2(uint type, uint j, uint k, uint l) {\n",
            "    switch (l) {\n",
            "        case 0: return getXYZLineIndex(type, j - 1u, k - 1u)[1]; break;//XY-plane[Xz]\n",
            "        case 1: return getXYZLineIndex(type, j - 1u, k)[1]; break;//XY-plane[XZ]\n",
            "        case 2: return getXYZLineIndex(type - 1u, j, k - 1u)[2]; break;//YZ-plane[xY]\n",
            "        case 3: return getXYZLineIndex(type, j, k - 1u)[2]; break;//YZ-plane[XY]\n",
            "        case 4: return getXYZLineIndex(type - 1u, j - 1u, k)[0]; break;//ZX-plane[yZ]\n",
            "        case 5: return getXYZLineIndex(type - 1u, j, k)[0]; break;//ZX-plane[YZ]\n",
            "        default: return 0;\n",
            "    }\n",
            "}\n",
            "int node3(uint type, uint j, uint k, uint l) {\n",
            "    switch (l) {\n",
            "        case 0: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[0]; break;//XY-plane[yz]\n",
            "        case 1: return getXYZLineIndex(type - 1u, j - 1u, k)[0]; break;//XY-plane[yZ]\n",
            "        case 2: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[1]; break;//YZ-plane[xz]\n",
            "        case 3: return getXYZLineIndex(type, j - 1u, k - 1u)[1]; break;//YZ-plane[Xz]\n",
            "        case 4: return getXYZLineIndex(type - 1u, j - 1u, k - 1u)[2]; break;//ZX-plane[xy]\n",
            "        case 5: return getXYZLineIndex(type - 1u, j, k - 1u)[2]; break;//ZX-plane[xY]\n",
            "        default: return 0;\n",
            "    }\n",
            "}\n",
            "float fOfXYZ(vec3 position) {\n",
            "    position -= currentOffset;\n",
            "    position *= distanceToOrigin;\n",
            "    for (int type = 0; type < valuesCounter; type++) {\n",
            "        switch (outBuffer.constants[type]) {\n",
            "            case 0: values[type] = outBuffer.equationValues[type]; break;\n",
            "            case X: values[type] = position.x; break;\n",
            "            case Y: values[type] = position.y; break;\n",
            "            case Z: values[type] = position.z; break;\n",
            "            case T: values[type] = t; break;\n",
            "            case E: values[type] = 2.718281828459045f; break;\n",
            "            case PI: values[type] = 3.141592653589793f; break;\n",
            "        }\n",
            "    }\n",
            "    for (int type = 0; type < sequenceLength; type++) {\n",
            "        switch (outBuffer.sequence[type][0]) {\n",
            "            case ADD: values[outBuffer.sequence[type][1]] += values[outBuffer.sequence[type][2]]; break;\n",
            "            case SUBTRACT: values[outBuffer.sequence[type][1]] -= values[outBuffer.sequence[type][2]]; break;\n",
            "            case MULTIPLY: values[outBuffer.sequence[type][1]] *= values[outBuffer.sequence[type][2]]; break;\n",
            "            case DIVIDE: values[outBuffer.sequence[type][1]] /= values[outBuffer.sequence[type][2]]; break;\n",
            "            case POWER: values[outBuffer.sequence[type][1]] = pow(values[outBuffer.sequence[type][1]], values[outBuffer.sequence[type][2]]); break;\n",
            "            case SINE: values[outBuffer.sequence[type][1]] = sin(values[outBuffer.sequence[type][1]]); break;\n",
            "            case COSINE: values[outBuffer.sequence[type][1]] = cos(values[outBuffer.sequence[type][1]]); break;\n",
            "            case TANGENT: values[outBuffer.sequence[type][1]] = tan(values[outBuffer.sequence[type][1]]); break;\n",
            "            case ARC_SINE: values[outBuffer.sequence[type][1]] = asin(values[outBuffer.sequence[type][1]]); break;\n",
            "            case ARC_COSINE: values[outBuffer.sequence[type][1]] = acos(values[outBuffer.sequence[type][1]]); break;\n",
            "            case ARC_TANGENT: values[outBuffer.sequence[type][1]] = atan(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_SINE: values[outBuffer.sequence[type][1]] = sinh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_COSINE: values[outBuffer.sequence[type][1]] = cosh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_TANGENT: values[outBuffer.sequence[type][1]] = tanh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_ARC_SINE: values[outBuffer.sequence[type][1]] = asinh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_ARC_COSINE: values[outBuffer.sequence[type][1]] = acosh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case HYPERBOLIC_ARC_TANGENT: values[outBuffer.sequence[type][1]] = atanh(values[outBuffer.sequence[type][1]]); break;\n",
            "            case ABSOLUTE_VALUE: values[outBuffer.sequence[type][1]] = abs(values[outBuffer.sequence[type][1]]); break;\n",
            "            case LOG: values[outBuffer.sequence[type][1]] = log(values[outBuffer.sequence[type][1]]) / log(10.0f); break;\n",
            "            case NATURAL_LOG: values[outBuffer.sequence[type][1]] = log(values[outBuffer.sequence[type][1]]); break;\n",
            "            case SQUARE_ROOT: values[outBuffer.sequence[type][1]] = sqrt(values[outBuffer.sequence[type][1]]); break;\n",
            "            case CUBED_ROOT: values[outBuffer.sequence[type][1]] = pow(values[outBuffer.sequence[type][1]], 1.0f / 3.0f); break;\n",
            "        }\n",
            "    }\n",
            "    return values[0];\n",
            "}\n",
            "void main() {\n",



            "    if(gl_LocalInvocationID.x == 0u && gl_LocalInvocationID.y == 0u){\n",
            "        for(int type = 0; type < 1024; type++){\n",
            "            for(int j = 0; j < 32; j++){\n",
            "                outBuffer.chunks[type].plusMinus[j] = true;\n",
            "            }\n",
            "        }\n",
            "    }\n",



            /*"    task = gl_WorkGroupSize.x * gl_LocalInvocationID.y + gl_LocalInvocationID.x;\n",
            "    // Erase normals\n",
            "    for (int type = int(3u * 32u * task); type < outBuffer.chunks[task].solutionCount; type++) {\n",
            "        outBuffer.chunks[task].vertices[type].n = vec3(0.0f);\n",
            "    }\n",
            "    // Reset solution particleCount\n",
            "    outBuffer.chunks[task].solutionCount = int(3u * 32u * task);\n",
            "    // Reset group segment counter\n",
            "    int groupSegmentCounter = 0;\n",
            "    \n",
            "    barrier();\n",
            "    \n",
            "    // Calculate plusMinusAmounts and approximate solutions\n",
            "    for (uint k = 0u; gl_LocalInvocationID.x < sizePlus3.x && gl_LocalInvocationID.y < sizePlus3.y && k < sizePlus3.z; k++) {\n",
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
            "                outBuffer.chunks[task].vertices[outBuffer.chunks[task].solutionCount].p[l] += scan;\n",
            "                if ((sign == POSITIVE) != (fOfXYZ(outBuffer.chunks[task].vertices[outBuffer.chunks[task].solutionCount].p) > 0.0f)) {\n",
            "                    scan *= 0.5f;\n",
            "                }else{\n",
            "                    sign *= -1;\n",
            "                    scan *= -0.5f;\n",
            "                }\n",
            "            }\n",
            "            outBuffer.chunks[task].solutionCount++;\n",
            "        }\n",
            "        if (gl_LocalInvocationID.x == 0u || gl_LocalInvocationID.y == 0u || k == 0u) {\n",
            "            continue;\n",
            "        }\n",
            "        int groupSegmentStartIndex = groupSegmentCounter;\n",
            "        int xyz = getPlusMinus(gl_LocalInvocationID.x - 1u, gl_LocalInvocationID.y - 1u, k - 1u);\n",
            "        int xyZ = getPlusMinus(gl_LocalInvocationID.x - 1u, gl_LocalInvocationID.y - 1u, k);\n",
            "        int xYz = getPlusMinus(gl_LocalInvocationID.x - 1u, gl_LocalInvocationID.y, k - 1u);\n",
            "        int xYZ = getPlusMinus(gl_LocalInvocationID.x - 1u, gl_LocalInvocationID.y, k);\n",
            "        int Xyz = getPlusMinus(gl_LocalInvocationID.x, gl_LocalInvocationID.y - gl_LocalInvocationID.y, k - 1u);\n",
            "        int XyZ = getPlusMinus(gl_LocalInvocationID.x, gl_LocalInvocationID.y - gl_LocalInvocationID.y, k);\n",
            "        int XYz = getPlusMinus(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k - 1u);\n",
            "        int XYZ = getPlusMinus(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k);\n",
            "        for (uint l = 0u; l < 6u; l++) {\n",
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
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 2://0010\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 3://0011\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
            "                case 4://0100\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 5://0101\n",
            "                    if(int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 6: {//0110\n",
            "                    int _node0 = node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node1 = node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node2 = node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node3 = node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    if (fOfXYZ((outBuffer.chunks[task].vertices[_node0].p + outBuffer.chunks[task].vertices[_node1].p + outBuffer.chunks[task].vertices[_node2].p + outBuffer.chunks[task].vertices[_node3].p) * 0.25f) > 0.0f) {\n",
            "                        if (int(l % 2u) != vectorPointsPositive){\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);\n",
            "                        }\n",
            "                    } else {\n",
            "                        if (int(l % 2u) != vectorPointsPositive) {\n",
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
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 8://1000\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    break;\n",
            "                case 9: {//1001\n",
            "                    int _node0 = node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node1 = node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node2 = node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    int _node3 = node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l);\n",
            "                    if (fOfXYZ((outBuffer.chunks[task].vertices[_node0].p + outBuffer.chunks[task].vertices[_node1].p + outBuffer.chunks[task].vertices[_node2].p + outBuffer.chunks[task].vertices[_node3].p) * 0.25f) > 0.0f) {\n",
            "                        if (int(l % 2u) != vectorPointsPositive) {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);\n",
            "                        } else {\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);\n",
            "                            groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);\n",
            "                        }\n",
            "                    } else {\n",
            "                        if (int(l % 2u) != vectorPointsPositive) {\n",
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
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
            "                case 11://1011\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node0(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
            "                case 12://1100\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
            "                case 13://1101\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node1(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
            "                case 14://1110\n",
            "                    if (int(l % 2u) != vectorPointsPositive)\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1);\n",
            "                    else\n",
            "                        groupSegments[groupSegmentCounter++] = ivec3(node3(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), node2(gl_LocalInvocationID.x, gl_LocalInvocationID.y, k, l), -1); break;\n",
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
            "        if (bool(clipEdges)) {\n",
            "            for (int l = groupSegmentStartIndex; l < groupSegmentCounter; l++) {\n",
            "                withinGraphRadius[l] = gl_LocalInvocationID.x > 1u && gl_LocalInvocationID.y > 1u && k > 1u && gl_LocalInvocationID.x < sizePlus2.x && gl_LocalInvocationID.y < sizePlus2.y && k < sizePlus2.z;\n",
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
            "                    vec3 sum = vec3(0.0f);\n",
            "                    for (int _n = l - segPerGroup + 2; _n < l + 2; _n++) {\n",
            "                        sum += outBuffer.chunks[task].vertices[groupSegments[_n][0]].p;\n",
            "                        groupSegments[_n][2] = outBuffer.chunks[task].solutionCount;\n",
            "                    }\n",
            "                    outBuffer.chunks[task].vertices[outBuffer.chunks[task].solutionCount].p = sum / float(segPerGroup);\n",
            "                    outBuffer.chunks[task].solutionCount++;\n",
            "                }\n",
            "                segPerGroup = 1;\n",
            "                l++;\n",
            "                break;\n",
            "            }\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    barrier();\n",
            "    \n",
            "    // Calculate face normals and add them to the per-solution normals.\n",
            "    for (int type = 0; type < groupSegmentCounter; type++) {\n",
            "        if (groupSegments[type][2] > -1) {\n",
            "            vec3 vectorA = outBuffer.chunks[task].vertices[groupSegments[type][0]].p - outBuffer.chunks[task].vertices[groupSegments[type][2]].p;\n",
            "            vec3 vectorB = outBuffer.chunks[task].vertices[groupSegments[type][1]].p - outBuffer.chunks[task].vertices[groupSegments[type][2]].p;\n",
            "            vec3 crossProduct = cross(vectorA, vectorB);\n",
            "            float length = dist(crossProduct);\n",
            "            vec3 normalizedCrossProduct = crossProduct / length;\n",
            "            outBuffer.chunks[task].vertices[groupSegments[type][0]].n += normalizedCrossProduct;\n",
            "            outBuffer.chunks[task].vertices[groupSegments[type][1]].n += normalizedCrossProduct;\n",
            "            outBuffer.chunks[task].vertices[groupSegments[type][2]].n += normalizedCrossProduct;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    barrier();\n",
            "    \n",
            "    //Normalize\n",
            "    for (int type = int(3u * 32u * task); type < outBuffer.chunks[task].solutionCount; type++) {\n",
            "        float length = bool(vectorPointsPositive) ? -dist(vec3(outBuffer.chunks[task].vertices[type].n)) : dist(vec3(outBuffer.chunks[task].vertices[type].n));\n",
            "        outBuffer.chunks[task].vertices[type].n /= length;\n",
            "    }\n",
            "    \n",
            "    barrier();\n",
            "    \n",
            "    //Create TriangleView Primitives\n",
            "    outBuffer.chunks[task].numIndices = 0;\n",
            "    int triangleCount = 0;\n",
            "    for (int type = 0; type < groupSegmentCounter; type++) {\n",
            "        if (groupSegments[type][2] > -1 && (!bool(clipEdges) || withinGraphRadius[type])) {\n",
            "            outBuffer.chunks[task].indices[triangleCount++] = uvec3(groupSegments[type]);\n",
            "            outBuffer.chunks[task].numIndices += 3;\n",
            "        }\n",
            "    }\n",
            "    \n",
            "    barrier();\n",
            "    \n",
            "    for (int type = int(3u * 32u * task); type < outBuffer.chunks[task].solutionCount; type++) {\n",
            "        outBuffer.chunks[task].vertices[type].p -= currentOffset;\n",
            "    }\n",*/
            "}"};

    string memoryEquation;

    void processEquation();

    explicit ImplicitGrapher(const ivec3& size, PositionXYZNormalXYZ*& vertices, uvec3*& indices, const bool& vectorPointsPositive);

    ~ImplicitGrapher();

    static size_t getRecommendedIndicesArraySize();

    static float fOfXYZ(vec3 position);

    void calculateSurfaceOnCPU(float (*fOfXYZ)(vec3), const float &timeVariable, const uint &iterations, const vec3 &offset, const float &zoom, const bool &clipEdges, PositionXYZNormalXYZ *_vertices, uvec3 *_indices, GLuint &_numIndices);

    void calculateSurfaceOnCPU(float (*fOfXYZ)(vec3, NaiveSimulation&, const vec3&, const vec3&), const float &timeVariable, const uint &iterations, const vec3 &offset, const float &zoom, const bool &clipEdges, PositionXYZNormalXYZ *_vertices, uvec3 *_indices, GLuint &_numIndices, NaiveSimulation& sim);

    ImplicitGrapher();

    // Assignment Constructor
    ImplicitGrapher(const ImplicitGrapher& other);

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other);

    vec3 defaultOffset;

    static int valuesCounter;

    static int hasTimeVariable;

    static int sequenceLength;

    static int constants[maxEquationLength];

    static float values[maxEquationLength];

    static float equationValues[maxEquationLength];

    static ivec3 sequences[maxEquationLength];

    static const int numOfDefaultEquations = 41;

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

    static vec3 currentOffset;

    static int maxSolutionCount;

    static int solutionCount;

    static int groupSegmentCounter;

    static float zoom;

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
