//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "ImplicitGrapher.h"

ImplicitGrapher::ImplicitGrapher(const ivec3& radius) {
    refactor(radius);
    plusMinus = (bool*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(bool));
    xyzLineIndex = (ivec3*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(ivec3));
    groupSegments = (ivec3*)malloc(maxSolutionCount * sizeof(ivec3));
    withinGraphRadius = (bool*)malloc(maxSolutionCount * sizeof(bool));
    vertices = (VertexNormal*)malloc(maxSolutionCount * sizeof(VertexNormal));
    indices = (uvec3*)malloc(3 * maxSolutionCount * sizeof(uvec3));
    writeDefaultEquationsToMemory();
    // writeUserEquationsToMemory();
    for (int i = 0; i < numOfEquationsInMemory; i++) {
        processEquation(i);
    }
}

ImplicitGrapher::~ImplicitGrapher(){
    free(plusMinus);
    free(xyzLineIndex);
    free(groupSegments);
    free(withinGraphRadius);
    free(vertices);
    free(indices);
}

// Assignment Constructor
ImplicitGrapher::ImplicitGrapher(const ImplicitGrapher& other) {
    // Copy primitive types
    debug_string = other.debug_string;
    t = other.t;
    maxSolutionCount = other.maxSolutionCount;
    surfaceEquation = other.surfaceEquation;
    solutionCount = other.solutionCount;
    groupSegmentCounter = other.groupSegmentCounter;
    numOfEquationsInMemory = other.numOfEquationsInMemory;

    // Copy arrays
    memcpy(plusMinus, other.plusMinus, maxNumOfEquations * sizeof(bool));
    memcpy(withinGraphRadius, other.withinGraphRadius, maxNumOfEquations * sizeof(bool));
    memcpy(xyzLineIndex, other.xyzLineIndex, maxNumOfEquations * sizeof(ivec3));
    memcpy(groupSegments, other.groupSegments, maxNumOfEquations * sizeof(ivec3));

    for (int i = 0; i < maxNumOfEquations; ++i) {
        memcpy(constant[i], other.constant[i], maxEquationLength * sizeof(int));
        valuesCounter[i] = other.valuesCounter[i];
        memcpy(values, other.values, maxEquationLength * sizeof(float));
        memcpy(equationValues[i], other.equationValues[i], maxEquationLength * sizeof(float));
        memcpy(sequences[i], other.sequences[i], maxEquationLength * 3 * sizeof(int));
        sequenceLengths[i] = other.sequenceLengths[i];
    }

    // Copy ivec3 and vec3 types
    radius = other.radius;
    radiusPlusOne = other.radiusPlusOne;
    size = other.size;
    sizePlus2 = other.sizePlus2;
    sizePlus3 = other.sizePlus3;
    defaultOffset = other.defaultOffset;
    currentOffset = other.currentOffset;

    // Copy dynamic arrays (if any) - adjust as needed
    vertices = (VertexNormal*)malloc(maxSolutionCount * sizeof(VertexNormal));
    memcpy(vertices, other.vertices, maxSolutionCount * sizeof(VertexNormal));

    indices = (uvec3*)malloc(maxSolutionCount * sizeof(uvec3));
    memcpy(indices, other.indices, maxSolutionCount * sizeof(uvec3));
}

// Copy Constructor
ImplicitGrapher&ImplicitGrapher::operator=(const ImplicitGrapher& other) {
    if (this != &other) {
        // Release existing resources if any

        // Copy-and-swap idiom for assignment operator
        ImplicitGrapher temp(other);
        std::swap(*this, temp);
    }
    return *this;
}

string ImplicitGrapher::decode(const int* const codedEquation, const int& length, const float* const values) {
    string equation = "";
    for (int i = 0; i < length; i++) {
        if (codedEquation[i] < 0) {
            switch (codedEquation[i]) {
                case X:
                    equation += "x";
                    break;
                case Y:
                    equation += "y";
                    break;
                case Z:
                    equation += "z";
                    break;
                case T:
                    equation += "t";
                    break;
                case E:
                    equation += "e";
                    break;
                case PI:
                    equation += pi;
                    break;
                case OPEN_PARENTHESIS:
                    equation += "(";
                    break;
                case CLOSE_PARENTHESIS:
                    equation += ")";
                    break;
                case ADD:
                    equation += "+";
                    break;
                case SUBTRACT:
                    equation += "-";
                    break;
                case MULTIPLY:
                    equation += "*";
                    break;
                case DIVIDE:
                    equation += "/";
                    break;
                case POWER:
                    equation += "^";
                    break;
                case SINE:
                    equation += "sin";
                    break;
                case COSINE:
                    equation += "cos";
                    break;
                case TANGENT:
                    equation += "tan";
                    break;
                case ARC_SINE:
                    equation += "asin";
                    break;
                case ARC_COSINE:
                    equation += "acos";
                    break;
                case ARC_TANGENT:
                    equation += "atan";
                    break;
                case LOG:
                    equation += "log";
                    break;
                case NATURAL_LOG:
                    equation += "ln";
                    break;
                case SQUARE_ROOT:
                    equation += "sqrt";
                    break;
                case CUBED_ROOT:
                    equation += "cbrt";
                    break;
                case UNDERSCORE:
                    equation += "_";
                    break;
            }
        }
        else {
            equation += std::to_string(values[codedEquation[i]]);
        }
    }
    return equation;
}

bool ImplicitGrapher::aDigit(const char& character) {
    return character >= '0' && character <= '9';
}

bool ImplicitGrapher::aCharacter(const char& character) {
    return
            character == 'x' ||
            character == 'y' ||
            character == 'z' ||
            character == 't' ||
            character == 'e' ||
            character == pi;
}

string ImplicitGrapher::charToString(const char* const characters, const int& length) {
    string string;
    for (int i = 0; i < length; i++) {
        string += characters[i];
    }
    return string;
}

void ImplicitGrapher::writeDefaultEquationsToMemory() {
    numOfEquationsInMemory = numOfDefaultEquations;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < numOfDefaultEquations; j++) {
            memoryEquations[j][i].assign(defaultEquations[j][i]);
        }
    }
}

void ImplicitGrapher::writeUserEquationsToMemory() {
    FILE* pFile = fopen("/sdcard/Android/data/com.newequationplotter02/files/user equations.txt", "r");
    if (pFile == NULL)
        return;
    char data[(int)pow(2, 16)];// char * data;
    int dataLengthMinusThree = (int)pow(2, 16) - 3;
    fgets(data, (int)pow(2, 16), pFile);
    for (int i = 0; !(data[i] == 'e' && data[i + 1] == 'o' && data[i + 2] == 'f') && i < dataLengthMinusThree; numOfEquationsInMemory++) {
        for (int j = 0; j < 2 && i < dataLengthMinusThree; j++) {
            while (data[i] != '~' && i < dataLengthMinusThree) {
                memoryEquations[numOfEquationsInMemory][j] += data[i];
                i++;
            }
            i++;
        }
    }
    fclose(pFile);
}

void ImplicitGrapher::processEquation(const int& i) {
    bool debugEquation = false;
    for (int j = 0; j < maxEquationLength; j++) {
        constant[i][j] = 0;
    }

    char equation[2 * maxEquationLength];
    int length = memoryEquations[i][EQUATION].length();

    // Convert string to char array
    for (int j = 0; j < length; j++) {
        equation[j] = memoryEquations[i][EQUATION].at(j);
    }
    // Remove spaces
    // Remove spaces and fill remainder with underscores");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string = "Remove spaces and fill remainder with underscores:";
    }
    valuesCounter[i] = 0;
    for (int j = 0; j < length; valuesCounter[i]++) {
        if (equation[valuesCounter[i]] != ' ') {
            equation[j] = equation[valuesCounter[i]];
            j++;
        }
        else {
            length--;
        }
    }

    // Fill remainder with underscores
    for (int j = length; j < 2 * maxEquationLength; j++) {
        equation[j] = '_';
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Isolate and convert '=' to '-'
    // Isolate and convert '=' to '-'");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Isolate and convert '=' to '-':";
    }
    equation[length + 3] = ')';
    valuesCounter[i] = length - 1;
    while (equation[valuesCounter[i]] != '=') {
        equation[valuesCounter[i] + 3] = equation[valuesCounter[i]];
        valuesCounter[i]--;
    }
    equation[valuesCounter[i] + 3] = '(';
    equation[valuesCounter[i] + 2] = '-';
    equation[valuesCounter[i] + 1] = ')';
    valuesCounter[i]--;
    while (valuesCounter[i] > -1) {
        equation[valuesCounter[i] + 1] = equation[valuesCounter[i]];
        valuesCounter[i]--;
    }
    equation[0] = '(';
    length += 4;
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to ends
    // Add parentheses to ends");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to ends:";
    }
    //Insert close parenthesis
    equation[length + 1] = ')';
    //Move characters
    for (int j = length; j > 0; j--) {
        equation[j] = equation[j - 1];
    }
    length += 2;
    //Insert open parenthesis
    equation[0] = '(';
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to functions
    // Add parentheses to functions:");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to functions:";
    }
    for (int j = 0; j < length - 1; j++) {//index equation
        bool functionFound = false;
        for (int k = 0; k < numOfFunctions && !functionFound; k++) {//index functions
            for (int l = j, functionLength = 0; l < length && !functionFound && equation[l] == functions[k][functionLength]; l++, functionLength++) {//index function characters
                if (equation[l] == '(') {
                    functionFound = true;
                    l++;
                    for (int parentheses = 1; parentheses > 0; l++) {
                        if (equation[l] == ')') {
                            parentheses--;
                        }
                        else if (equation[l] == '(') {
                            parentheses++;
                        }
                    }
                    if (!(equation[j - 1] == '(' && equation[l] == ')')) {
                        length += 2;
                        //Move characters
                        for (int m = length - 1; m > l + 1; m--) {
                            equation[m] = equation[m - 2];
                        }
                        //Insert close parenthesis
                        equation[l + 1] = ')';
                        //Move characters
                        for (int m = l; m > j; m--) {
                            equation[m] = equation[m - 1];
                        }
                        //Insert open parenthesis
                        equation[j] = '(';
                    }
                    j += functionLength;
                }
            }
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add multiplication signs to close terms
    // Add multiplication signs to close terms:");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add multiplication signs to close terms:";
    }
    for (int j = 0; j < length; j++) {
        bool notR = true;
        if (j > 0) {
            if (equation[j - 1] == 'r') {
                notR = false;
            }
        }
        if (
                (
                        equation[j] == ')' ||
                        aDigit(equation[j]) ||
                        (
                                aCharacter(equation[j]) &&
                                (
                                        equation[j] != 't' || notR
                                )
                        )
                ) && (
                        aCharacter(equation[j + 1]) ||
                        equation[j + 1] == '(' ||
                        aDigit(equation[j + 1])
                ) && (
                        !aDigit(equation[j]) ||
                        !aDigit(equation[j + 1])
                )
                ) {
            for (int k = length; k > j; k--) {
                equation[k] = equation[k - 1];
            }
            equation[j + 1] = '*';
            length++;
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Convert (-x to (0-x or (-1 to (0-1
    // Convert (-x to ((0-x) or (-1 to ((0-1)");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Convert (-x to ((0-x) or (-1 to ((0-1):";
    }
    for (int j = 0; j < length; j++) {
        if (equation[j] == '(' && equation[j + 1] == '-') {
            //Move characters
            length++;
            for (int k = length - 1; k > j + 1; k--) {
                equation[k] = equation[k - 1];
            }
            //Insert zero
            equation[j + 1] = '0';
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to symbol functions
    // Add parentheses to symbol functions");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to symbol functions:";
    }
    for (int sweep = 1; sweep < 4; sweep++) {
        int whatJEquals, increment;
        if (sweep == 1) {
            whatJEquals = length - 1;
            increment = -1;
        }
        else {
            whatJEquals = 1;
            increment = 1;
        }
        for (int j = whatJEquals; (sweep == 1 && j > 0) || (sweep > 1 && j < length); j += increment) {//Index equation
            if (
                    (
                            sweep == 1 && equation[j] == '^'
                    ) || (
                            sweep == 2 && (equation[j] == '*' || equation[j] == '/')
                    ) || (
                            sweep == 3 && (equation[j] == '+' || equation[j] == '-')
                    )
                    ) {
                int mode = 0;
                const int DIGIT = 1;
                const int LETTER = 2;
                const int PARENTHESIS = 3;
                int positions[2] = { 0, 0 };
                for (int k = 1, position = 0; k > -2; k -= 2, position++) {
                    if (aDigit(equation[j + k]) || equation[j + k] == '.') {
                        mode = DIGIT;
                    }
                    else if (aCharacter(equation[j + k])) {
                        mode = LETTER;
                    }
                    else if (
                            (equation[j + k] == '(' && k == 1) ||
                            (equation[j + k] == ')' && k == -1)
                            ) {
                        mode = PARENTHESIS;
                    }
                    positions[position] = j + 2 - 4 * position;
                    switch (mode) {
                        case DIGIT:
                            while (
                                    aDigit(equation[positions[position]]) ||
                                    equation[positions[position]] == '.'
                                    ) {
                                positions[position] += k;
                            }
                            break;
                        case LETTER:
                            break;
                        case PARENTHESIS:
                            for (int parentheses = 1; parentheses > 0; positions[position] += k) {
                                if (equation[positions[position]] == ')') {
                                    parentheses -= k;
                                }
                                else if (equation[positions[position]] == '(') {
                                    parentheses += k;
                                }
                            }
                            break;
                    }
                }
                if (!(equation[positions[1]] == '(' && equation[positions[0]] == ')')) {
                    length += 2;
                    //Move characters
                    for (int k = length - 1; k > positions[0] + 1; k--) {
                        equation[k] = equation[k - 2];
                    }
                    //Insert close parenthesis
                    equation[positions[0] + 1] = ')';
                    //Move characters
                    for (int k = positions[0]; k > positions[1]; k--) {
                        equation[k] = equation[k - 1];
                    }
                    //Insert close parenthesis
                    equation[positions[1] + 1] = '(';
                }
            }
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length) + ", ";
    }

    //Convert to coded equation
    // Convert to coded equation");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Convert to coded equation:";
    }
    int codedEquation[2 * maxEquationLength];
    int codedEquationCounter = 0;
    valuesCounter[i] = 0;
    for (int j = 0; j < length; j++, codedEquationCounter++) {
        if (equation[j] != '.' && !aDigit(equation[j])) {
            if (equation[j] == 'x') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = X;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'y') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = Y;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'z') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = Z;
                valuesCounter[i]++;
            }
            else if (equation[j] == 't' && equation[j + 1] != 'a') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = T;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'e') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = E;
                valuesCounter[i]++;
            }
            else if (equation[j] == pi) {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = PI;
                valuesCounter[i]++;
            }
            else if (equation[j] == '(') {
                codedEquation[codedEquationCounter] = OPEN_PARENTHESIS;
            }
            else if (equation[j] == ')') {
                codedEquation[codedEquationCounter] = CLOSE_PARENTHESIS;
            }
            else if (equation[j] == '+') {
                codedEquation[codedEquationCounter] = ADD;
            }
            else if (equation[j] == '-') {
                codedEquation[codedEquationCounter] = SUBTRACT;
            }
            else if (equation[j] == '*') {
                codedEquation[codedEquationCounter] = MULTIPLY;
            }
            else if (equation[j] == '/') {
                codedEquation[codedEquationCounter] = DIVIDE;
            }
            else if (equation[j] == '^') {
                codedEquation[codedEquationCounter] = POWER;
            }
            else if (equation[j] == functions[0][0] && equation[j + 1] == functions[0][1]) {
                if (equation[j + 3] == functions[0][3]) {
                    codedEquation[codedEquationCounter] = SINE;
                    j += 2;
                }
                else if (equation[j + 3] == functions[6][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_SINE;
                    j += 3;
                }
            }
            else if (equation[j] == functions[1][0] && equation[j + 1] == functions[1][1]) {
                if (equation[j + 3] == functions[1][3]) {
                    codedEquation[codedEquationCounter] = COSINE;
                    j += 2;
                }
                else if (equation[j + 3] == functions[7][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_COSINE;
                    j += 3;
                }
            }
            else if (equation[j] == functions[2][0] && equation[j + 1] == functions[2][1]) {
                if (equation[j + 3] == functions[2][3]) {
                    codedEquation[codedEquationCounter] = TANGENT;
                    j += 2;
                }
                else if (equation[j + 3] == functions[8][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_TANGENT;
                    j += 3;
                }
            }
            else if (equation[j] == functions[3][0] && equation[j + 1] == functions[3][1]) {
                if (equation[j + 4] == functions[3][4]) {
                    codedEquation[codedEquationCounter] = ARC_SINE;
                    j += 3;
                }
                else if (equation[j + 4] == functions[9][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_SINE;
                    j += 4;
                }
            }
            else if (equation[j] == functions[4][0] && equation[j + 1] == functions[4][1]) {
                if (equation[j + 4] == functions[4][4]) {
                    codedEquation[codedEquationCounter] = ARC_COSINE;
                    j += 3;
                }
                else if (equation[j + 4] == functions[10][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_COSINE;
                    j += 4;
                }
            }
            else if (equation[j] == functions[5][0] && equation[j + 1] == functions[5][1]) {
                if (equation[j + 4] == functions[5][4]) {
                    codedEquation[codedEquationCounter] = ARC_TANGENT;
                    j += 3;
                }
                else if (equation[j + 4] == functions[11][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_TANGENT;
                    j += 4;
                }
            }
            else if (equation[j] == functions[12][0] && equation[j + 1] == functions[12][1]) {
                codedEquation[codedEquationCounter] = ABSOLUTE_VALUE;
                j += 2;
            }
            else if (equation[j] == functions[13][0] && equation[j + 1] == functions[13][1]) {
                codedEquation[codedEquationCounter] = LOG;
                j += 2;
            }
            else if (equation[j] == functions[14][0] && equation[j + 1] == functions[14][1]) {
                codedEquation[codedEquationCounter] = NATURAL_LOG;
                j += 1;
            }
            else if (equation[j] == functions[15][0] && equation[j + 1] == functions[15][1]) {
                codedEquation[codedEquationCounter] = SQUARE_ROOT;
                j += 3;
            }
            else if (equation[j] == functions[16][0] && equation[j + 1] == functions[16][1]) {
                codedEquation[codedEquationCounter] = CUBED_ROOT;
                j += 3;
            }
            else if (equation[j] == '_') {
                codedEquation[codedEquationCounter] = UNDERSCORE;
            }
        }
        else {
            int numberSize = 0;
            for (int k = j; equation[k] == '.' || aDigit(equation[k]); k++) {
                numberSize++;
            }
            float number = 0;
            int power = 0;
            for (int jMinusOne = j - 1, k = jMinusOne + numberSize; k > jMinusOne; k--, power++) {
                if (equation[k] == '.') {
                    number *= pow(10, -power);
                    power = -1;
                }
                else if (aDigit(equation[k])) {
                    number += (equation[k] - 48) * pow(10, power);
                }
            }
            codedEquation[codedEquationCounter] = valuesCounter[i];
            equationValues[i][valuesCounter[i]] = values[valuesCounter[i]] = number;
            valuesCounter[i]++;
            j += numberSize - 1;
        }
    }
    //Display equation
    // original equation = " + PlotterSettings.defaultEquations[i][EQUATION]);
    // array = " + charToString(equation));
    // coded equation = " + decode(codedEquation, values));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += decode(codedEquation, codedEquationCounter, values);
    }

    //Convert 0-2 to -2
    //Convert 2-0 to 2
    //Convert 0+2 to 2
    //Convert 2+0 to 2
    //Convert 0*2 to 0
    //Convert 2*0 to 0
    //Convert 0/2 to 0
    //Convert 2/0 to error

    //Create Sequence Table destroying coded equation
    // Create Sequence Table destroying coded equation:");
    for (int j = 0, counter = 0; j < codedEquationCounter; j++) {
        if (codedEquation[j] > -1) {
            counter++;
        }
        else if (codedEquation[j] > -7) {
            codedEquation[j] = counter;
            counter++;
        }
    }
    // coded equation = " + decode(codedEquation, values));
    bool end = false;
    for (sequenceLengths[i] = 0; !end; ) {
        for (int j = 0, openParenthesis = 0; j < codedEquationCounter; j++) {
            if (j > codedEquationCounter - 2) {
                end = true;
            }
            switch (codedEquation[j]) {
                case OPEN_PARENTHESIS:
                    openParenthesis = j;
                    break;
                case CLOSE_PARENTHESIS:
                    switch (j - openParenthesis) {
                        case 1:// "()"
                            for (int k = j - 1; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 2];
                            }
                        case 2:// "(a)"
                            codedEquation[j - 2] = codedEquation[j - 1];
                            for (int k = j - 1; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 2];
                            }
                            break;
                        case 3: {// "(fa)"
                            sequences[i][sequenceLengths[i]][0] = codedEquation[j - 2];//function
                            sequences[i][sequenceLengths[i]][1] = codedEquation[j - 1];//where first value is and where result will be stored
                            codedEquation[j - 3] = codedEquation[j - 1];
                            for (int k = j - 2; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 3];
                            }
                            sequenceLengths[i]++; }
                            break;
                        case 4: {// "(a+b)" "(a-b)" "(a*b)" "(a/b)" "(a^b)"
                            sequences[i][sequenceLengths[i]][0] = codedEquation[j - 2];//function
                            sequences[i][sequenceLengths[i]][1] = codedEquation[j - 3];//where first value is and where result will be stored
                            sequences[i][sequenceLengths[i]][2] = codedEquation[j - 1];//where second value is
                            codedEquation[j - 4] = codedEquation[j - 3];
                            for (int k = j - 3; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 4];
                            }
                            sequenceLengths[i]++; }
                            break;
                    }
                    // coded equation = " + decode(codedEquation, values));
                    j = codedEquationCounter - 1;
                    break;
            }
        }
    }
}

inline int ImplicitGrapher::node0(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j, k - 1)[0]; break;//XY-plane[Yz]
        case 1: return getXYZLineIndex(i - 1, j, k)[0]; break;//XY-plane[YZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k)[1]; break;//YZ-plane[xZ]
        case 3: return getXYZLineIndex(i, j - 1, k)[1]; break;//YZ-plane[XZ]
        case 4: return getXYZLineIndex(i, j - 1, k - 1)[2]; break;//ZX-plane[Xy]
        case 5: return getXYZLineIndex(i, j, k - 1)[2]; break;//ZX-plane[XY]
        default: return 0;
    }
}

inline int ImplicitGrapher::node1(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j - 1, k - 1)[1]; break;//XY-plane[xz]
        case 1: return getXYZLineIndex(i - 1, j - 1, k)[1]; break;//XY-plane[xZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k - 1)[2]; break;//YZ-plane[xy]
        case 3: return getXYZLineIndex(i, j - 1, k - 1)[2]; break;//YZ-plane[Xy]
        case 4: return getXYZLineIndex(i - 1, j - 1, k - 1)[0]; break;//ZX-plane[yz]
        case 5: return getXYZLineIndex(i - 1, j, k - 1)[0]; break;//ZX-plane[Yz]
        default: return 0;
    }
}

inline int ImplicitGrapher::node2(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i, j - 1, k - 1)[1]; break;//XY-plane[Xz]
        case 1: return getXYZLineIndex(i, j - 1, k)[1]; break;//XY-plane[XZ]
        case 2: return getXYZLineIndex(i - 1, j, k - 1)[2]; break;//YZ-plane[xY]
        case 3: return getXYZLineIndex(i, j, k - 1)[2]; break;//YZ-plane[XY]
        case 4: return getXYZLineIndex(i - 1, j - 1, k)[0]; break;//ZX-plane[yZ]
        case 5: return getXYZLineIndex(i - 1, j, k)[0]; break;//ZX-plane[YZ]
        default: return 0;
    }
}

inline int ImplicitGrapher::node3(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j - 1, k - 1)[0]; break;//XY-plane[yz]
        case 1: return getXYZLineIndex(i - 1, j - 1, k)[0]; break;//XY-plane[yZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k - 1)[1]; break;//YZ-plane[xz]
        case 3: return getXYZLineIndex(i, j - 1, k - 1)[1]; break;//YZ-plane[Xz]
        case 4: return getXYZLineIndex(i - 1, j - 1, k - 1)[2]; break;//ZX-plane[xy]
        case 5: return getXYZLineIndex(i - 1, j, k - 1)[2]; break;//ZX-plane[xY]
        default: return 0;
    }
}

// Calculates the difference between f(x, y, z) and 0.
float ImplicitGrapher::fOfXYZ(ImplicitGrapher &graph, vec3 _, const float &t, const vec3 &offset, const float &zoom) {
    _ -= offset;
    _ *= zoom;
    for (int i = 0; i < graph.valuesCounter[graph.surfaceEquation]; i++) {
        switch (graph.constant[graph.surfaceEquation][i]) {
            case 0: graph.values[i] = graph.equationValues[graph.surfaceEquation][i]; break;
            case X: graph.values[i] = _.x; break;
            case Y: graph.values[i] = _.y; break;
            case Z: graph.values[i] = _.z; break;
            case T: graph.values[i] = t; break;
            case E: graph.values[i] = M_E; break;
            case PI: graph.values[i] = M_PI; break;
        }
    }
    // A better solution to below may be near the bottom of the following web page: https://www.cplusplus.com/doc/tutorial/pointers/
    int e = graph.surfaceEquation;
    float* v = graph.values;
    for (int i = 0; i < graph.sequenceLengths[e]; i++) {
        int* s = graph.sequences[e][i];
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
}

void ImplicitGrapher::calculateSurfaceOnCPU(float (*fOfXYZ)(ImplicitGrapher& graph, vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, VertexNormal* _vertices, uvec3* _indices, GLuint& _numIndices) {
    t = timeVariable;
    currentOffset = defaultOffset + offset;
    // Erase normals
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].n = vec3(0.0f);
    }
    // Reset solution count
    solutionCount = 0;
    // Reset group segment counter
    groupSegmentCounter = 0;
    // Calculate plusMinusAmounts and approximate solutions
    for (int i = 0; i < sizePlus3.x; i++) {
        for (int j = 0; j < sizePlus3.y; j++) {
            for (int k = 0; k < sizePlus3.z; k++) {
                plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k] = fOfXYZ(*this, vec3(i, j, k), t, currentOffset, zoom) > 0.0f;
                uvec3 ijk(i, j, k);
                for (int l = 0; l < 3; l++) {
                    if (ijk[l] == 0) {
                        continue;
                    }
                    uvec3 iCursor = ijk;
                    iCursor[l] -= 1;
                    bool firstSample = plusMinus[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z];
                    bool secondSample = plusMinus[(ijk.x * sizePlus3.y + ijk.y) * sizePlus3.z + ijk.z];
                    // If no solution is detected jump to next iteration of loop.
                    if (!(firstSample ^ secondSample)) {
                        continue;
                    }
                    xyzLineIndex[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z][l] = solutionCount;
                    const int POSITIVE = 1;
                    const int NEGATIVE = -1;
                    int sign = secondSample ? POSITIVE : NEGATIVE;
                    for (int m = 0; m < 3; m++) {
                        _vertices[solutionCount].v[m] = iCursor[m];
                    }
                    float scan = 0.5f;
                    for (int m = 0; m < iterations; m++) {// Maybe use a do-while loop here to reduce the scan operations by 1.
                        _vertices[solutionCount].v[l] += scan;
                        if (sign == POSITIVE ^ fOfXYZ(*this, _vertices[solutionCount].v, t, currentOffset, zoom) > 0.0f) {
                            scan *= 0.5f;
                        }else{
                            sign *= -1;
                            scan *= -0.5f;
                        }
                    }
                    solutionCount++;
                }
                if (i == 0 || j == 0 || k == 0) {
                    continue;
                }
                int groupSegmentStartIndex = groupSegmentCounter;
                bool xyz = getPlusMinus(i - 1, j - 1, k - 1);
                bool xyZ = getPlusMinus(i - 1, j - 1, k);
                bool xYz = getPlusMinus(i - 1, j, k - 1);
                bool xYZ = getPlusMinus(i - 1, j, k);
                bool Xyz = getPlusMinus(i, j - 1, k - 1);
                bool XyZ = getPlusMinus(i, j - 1, k);
                bool XYz = getPlusMinus(i, j, k - 1);
                bool XYZ = getPlusMinus(i, j, k);
                for (int l = 0; l < 6; l++) {
                    int combo = 0;
                    switch (l) {
                        case 0://XY
                            combo += xYz * 8;
                            combo += XYz * 4;
                            combo += xyz * 2;
                            combo += Xyz * 1;
                            break;
                        case 1://XY
                            combo += xYZ * 8;
                            combo += XYZ * 4;
                            combo += xyZ * 2;
                            combo += XyZ * 1;
                            break;
                        case 2://YZ
                            combo += xyZ * 8;
                            combo += xYZ * 4;
                            combo += xyz * 2;
                            combo += xYz * 1;
                            break;
                        case 3://YZ
                            combo += XyZ * 8;
                            combo += XYZ * 4;
                            combo += Xyz * 2;
                            combo += XYz * 1;
                            break;
                        case 4://ZX
                            combo += Xyz * 8;
                            combo += XyZ * 4;
                            combo += xyz * 2;
                            combo += xyZ * 1;
                            break;
                        case 5://ZX
                            combo += XYz * 8;
                            combo += XYZ * 4;
                            combo += xYz * 2;
                            combo += xYZ * 1;
                            break;
                    }
                    switch (combo) {
                        case 0: break;//0000
                        case 1:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            break;//0001
                        case 2:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            break;//0010
                        case 3:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1); break;//0011
                        case 4:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            break;//0100
                        case 5:
                            if(l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            break;//0101
                        case 6: {//0110
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ(*this, (_vertices[_node0].v + _vertices[_node1].v + _vertices[_node2].v + _vertices[_node3].v) * 0.25f, t, currentOffset, zoom) > 0.0f) {
                                if (l % 2 != vectorPointsPositive){
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);
                                }
                            } else {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);
                                }
                            }
                            break;
                        }
                        case 7:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            break;//0111
                        case 8:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            break;//1000
                        case 9: {//1001
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ(*this, (_vertices[_node0].v + _vertices[_node1].v + _vertices[_node2].v + _vertices[_node3].v) * 0.25f, t, currentOffset, zoom) > 0.0f) {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);
                                }
                            } else {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);
                                }
                            }
                            break;
                        }
                        case 10:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1); break;//1010
                        case 11:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1); break;//1011
                        case 12:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1); break;//1100
                        case 13:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1); break;//1101
                        case 14:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1); break;//1110
                        case 15:
                            break;//1111
                    }
                }
                //Grouping
                //{73}{64}{0}    {73}{64}{0}    {73}{64}{107}
                //{97}{21}{0}    {64}{89}{0}    {64}{89}{107}
                //{89}{64}{0}    {89}{80}{0}    {89}{80}{107}
                //{89}{80}{0}    {80}{73}{0}    {80}{73}{107}
                //{54}{21}{0}    {21}{97}{0}    {21}{97}{54}
                //{54}{97}{0}    {97}{54}{0}    {97}{54}{0}
                //{80}{73}{0}    {54}{21}{0}    {54}{21}{0}
                int segPerGroup = 1;
                if (clipEdges) {
                    for (int l = groupSegmentStartIndex; l < groupSegmentCounter; l++) {
                        withinGraphRadius[l] = i > 1 && j > 1 && k > 1 && i < sizePlus2.x && j < sizePlus2.y && k < sizePlus2.z;
                    }
                }
                for (int l = groupSegmentStartIndex; l < groupSegmentCounter - 1; l++) {
                    for (int _m = l + 1; _m < groupSegmentCounter; _m++) {
                        if (groupSegments[_m][0] != groupSegments[l][1]) {
                            continue;
                        }
                        //Swap
                        int storeA = groupSegments[l + 1][0];
                        int storeB = groupSegments[l + 1][1];
                        groupSegments[l + 1][0] = groupSegments[_m][0];
                        groupSegments[l + 1][1] = groupSegments[_m][1];
                        groupSegments[_m][0] = storeA;
                        groupSegments[_m][1] = storeB;
                        segPerGroup++;
                        //Check to see if end of loop
                        if (groupSegments[l + 1][1] != groupSegments[l + 2 - segPerGroup][0]) {
                            continue;
                        }
                        if (segPerGroup == 3) {
                            groupSegments[l - 1][2] = groupSegments[l][1];
                        }
                        else {
                            vec3 sum(0.0f);
                            for (int _n = l - segPerGroup + 2; _n < l + 2; _n++) {
                                sum += _vertices[groupSegments[_n][0]].v;
                                groupSegments[_n][2] = solutionCount;
                            }
                            _vertices[solutionCount].v = sum / segPerGroup;
                            solutionCount++;
                        }
                        segPerGroup = 1;
                        l++;
                        break;
                    }
                }
            }
        }
    }

    // Calculate face normals and add them to the per-solution normals.
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1) {
            vec3 vectorA = _vertices[groupSegments[i][0]].v - _vertices[groupSegments[i][2]].v;
            vec3 vectorB = _vertices[groupSegments[i][1]].v - _vertices[groupSegments[i][2]].v;
            vec3 crossProduct = cross(vectorA, vectorB);
            float length = distance(crossProduct);
            vec3 normalizedCrossProduct = crossProduct / length;
            _vertices[groupSegments[i][0]].n += normalizedCrossProduct;
            _vertices[groupSegments[i][1]].n += normalizedCrossProduct;
            _vertices[groupSegments[i][2]].n += normalizedCrossProduct;
        }
    }
    //Normalize
    for (int i = 0; i < solutionCount; i++) {
        float length = (vectorPointsPositive) ? -distance(vec3(_vertices[i].n)) : distance(vec3(_vertices[i].n));
        _vertices[i].n /= length;
    }
    //Create Triangle Primitives
    _numIndices = 0;
    int triangleCount = 0;
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1 && (!clipEdges || withinGraphRadius[i])) {
            _indices[triangleCount++] = groupSegments[i];
            _numIndices += 3;
        }
    }
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].v -= currentOffset;
    }
}

bool ImplicitGrapher::getPlusMinus(const int& i, const int& j, const int& k){
    return plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

inline ivec3 ImplicitGrapher::getXYZLineIndex(const int& i, const int& j, const int& k){
    return xyzLineIndex[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

void ImplicitGrapher::refactor(const ivec3& inputRadius) {
    radius = inputRadius;
    radiusPlusOne = radius + ivec3(1);
    size = radius * 2;
    sizePlus2 = size + ivec3(2);
    sizePlus3 = size + ivec3(3);
    defaultOffset = vec3(radiusPlusOne);
    maxSolutionCount = 3 * sizePlus3.x * sizePlus3.y * sizePlus3.z;
}