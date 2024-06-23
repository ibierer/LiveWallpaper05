//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "ImplicitGrapher.h"

double ImplicitGrapher::t = 0.0;

bool* ImplicitGrapher::plusMinus = nullptr;

int ImplicitGrapher::constants[maxEquationLength] = {};

int ImplicitGrapher::valuesCounter = 0;

int ImplicitGrapher::hasTimeVariable = 0;

string ImplicitGrapher::debug_string = "";

float ImplicitGrapher::equationValues[maxEquationLength] = {};

float ImplicitGrapher::values[maxEquationLength] = {};

ivec3 ImplicitGrapher::sequences[maxEquationLength] = {};

int ImplicitGrapher::sequenceLength = 0;

vec3 ImplicitGrapher::currentOffset = vec3(0.0f);

int ImplicitGrapher::solutionCount = 0;

int ImplicitGrapher::groupSegmentCounter = 0;

ivec3* ImplicitGrapher::xyzLineIndex = nullptr;

ivec3* ImplicitGrapher::groupSegments = nullptr;

bool* ImplicitGrapher::withinGraphRadius = nullptr;

int ImplicitGrapher::maxSolutionCount = 0;

float ImplicitGrapher::zoom = 0.0f;

//ImplicitGrapher::GPUdata* ImplicitGrapher::data = nullptr;

const string ImplicitGrapher::functions[numOfFunctions] = {
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

ImplicitGrapher::ImplicitGrapher() {

}

ImplicitGrapher::ImplicitGrapher(const ivec3& inputSize, PositionXYZNormalXYZ*& vertices, uvec3*& indices, const bool& vectorPointsPositive) {
    refactor(inputSize);
    this->vectorPointsPositive = vectorPointsPositive;
    plusMinus = (bool*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(bool));
    xyzLineIndex = (ivec3*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(ivec3));
    groupSegments = (ivec3*)malloc(maxSolutionCount * sizeof(ivec3));
    withinGraphRadius = (bool*)malloc(maxSolutionCount * sizeof(bool));
    vertices = (PositionXYZNormalXYZ*)malloc(maxSolutionCount * sizeof(PositionXYZNormalXYZ));
    indices = (uvec3*)malloc(getRecommendedIndicesArraySize());

    //computeShaderProgram = View::createComputeShaderProgram(View::stringArrayToString((string*)computeShaderCode, 1000).c_str());
    //glGenBuffers(1, &computeShaderVBO);
    //data = (GPUdata*)malloc(sizeof(GPUdata));
}

ImplicitGrapher::~ImplicitGrapher(){

}

// Assignment Constructor
ImplicitGrapher::ImplicitGrapher(const ImplicitGrapher& other) {

}

// Copy Constructor
ImplicitGrapher&ImplicitGrapher::operator=(const ImplicitGrapher& other) {
    if (this != &other) {
        defaultOffset = other.defaultOffset;
        vectorPointsPositive = other.vectorPointsPositive;
        size = other.size;
        sizePlus2 = other.sizePlus2;
        sizePlus3 = other.sizePlus3;
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

void ImplicitGrapher::convertPiSymbol(string &equation) {
    string piString = "π";
    string enquiryString = "";
    for (int32_t pos = equation.find(piString.c_str(), 0); pos != string::npos; pos = equation.find(piString.c_str(), pos + enquiryString.length())) {
        equation.replace(pos, piString.length(), enquiryString.c_str(), enquiryString.length());
    }
    piString = "pi";
    enquiryString = "";
    for (int32_t pos = equation.find(piString.c_str(), 0); pos != string::npos; pos = equation.find(piString.c_str(), pos + enquiryString.length())) {
        equation.replace(pos, piString.length(), enquiryString.c_str(), enquiryString.length());
    }
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

bool ImplicitGrapher::anOperator(const char& character){
    return
            character == '^' ||
            character == '/' ||
            character == '*' ||
            character == '-' ||
            character == '+';
}

string ImplicitGrapher::checkEquationSyntax(const string& editable){
    string syntaxError = "";
    bool containsEqualSign = false;
    int parenthesisCountLeft = 0;
    int parenthesisCountRight = 0;
    bool improperParenthesisLeft = false;
    bool improperParenthesisRight = false;
    for(int i = 0; i < editable.length(); i++){
        if(editable.at(i) == '('){
            if(containsEqualSign){
                parenthesisCountRight++;
            }else{
                parenthesisCountLeft++;
            }
        }else if(editable.at(i) == ')'){
            if(containsEqualSign){
                parenthesisCountRight--;
            }else{
                parenthesisCountLeft--;
            }
        }else if(editable.at(i) == '='){
            if(containsEqualSign){
                syntaxError = "Syntax Error:  contains more than one equal sign.";
                return syntaxError;
            }else{
                containsEqualSign = true;
            }
        }
        if(parenthesisCountLeft < 0){
            improperParenthesisLeft = true;
        }else if(parenthesisCountRight < 0){
            improperParenthesisRight = true;
        }
    }
    if(!containsEqualSign){
        syntaxError = "Syntax Error:  lacks an equal sign.";
        return syntaxError;
    }
    if(improperParenthesisLeft || improperParenthesisRight){
        syntaxError = "Syntax Error:  improper parenthesis on ";
        if(!improperParenthesisLeft){
            syntaxError += "right.";
        }else if(improperParenthesisRight){
            syntaxError += "both sides.";
        }else{
            syntaxError += "left.";
        }
        return syntaxError;
    }
    if(parenthesisCountLeft != 0 || parenthesisCountRight != 0){
        syntaxError = "Syntax Error:  invalid parenthesis count on ";
        if(parenthesisCountLeft == 0){
            syntaxError += "right.";
        }else if(parenthesisCountRight != 0){
            syntaxError += "both sides.";
        }else{
            syntaxError += "left.";
        }
        return syntaxError;
    }

    //Check for expressions
    bool termOnLeft = false;
    bool termOnRight = false;
    bool passedEqualSign = false;
    for(int i = 0; i < editable.length() && !termOnRight; i++){
        if(aDigit(editable.at(i)) || aCharacter(editable.at(i))){
            if(passedEqualSign){
                termOnRight = true;
            }else{
                termOnLeft = true;
            }
        }else if(editable.at(i) == '='){
            passedEqualSign = true;
        }
    }
    if(!termOnLeft || !termOnRight){
        syntaxError = "Syntax Error:  no expression on ";
        if(termOnLeft){
            syntaxError += "right side.";
        }else if(!termOnRight){
            syntaxError += "either side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Improper term I
    bool badTermOnLeft = false;
    bool badTermOnRight = false;
    passedEqualSign = false;
    for(int i = 0; i < editable.length() - 1 && !badTermOnRight; i++){
        if(
                aCharacter(editable.at(i)) &&
                (
                        editable.at(i + 1) == '.' ||
                        aDigit(editable.at(i + 1))
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }else if(editable.at(i) == '='){
            passedEqualSign = true;
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper term on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Convert to noSpaces
    string noSpaces = "";
    for(int i = 0; i < editable.length(); i++){
        if(editable.at(i) != ' '){
            noSpaces += editable.at(i);
        }
    }

    //Improper term II
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        noSpaces.at(i) == '(' &&
                        (
                                noSpaces.at(i + 1) == ')' ||
                                (
                                        anOperator(noSpaces.at(i + 1)) &&
                                        noSpaces.at(i + 1) != '-'
                                )
                        )
                ) || (
                        (
                                noSpaces.at(i) == '(' ||
                                anOperator(noSpaces.at(i))
                        ) &&
                        noSpaces.at(i + 1) == ')'
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper term on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Improper Decimal
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        noSpaces.at(i) == '.' &&
                        !aDigit(noSpaces.at(i + 1))
                ) ||
                (
                        i == noSpaces.length() - 2 &&
                        noSpaces.at(noSpaces.length() - 1) == '.'
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper decimal on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Invalid Operator
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        i == noSpaces.length() - 2 &&
                        anOperator(noSpaces.at(noSpaces.length() - 1))
                ) || (
                        anOperator(noSpaces.at(i)) &&
                        (
                                noSpaces.at(i + 1) == ')' ||
                                noSpaces.at(i + 1) == '=' ||
                                (
                                        i == 0 &&
                                        noSpaces.at(i) != '-'
                                )
                        )
                ) || (
                        (
                                noSpaces.at(i) == '(' ||
                                noSpaces.at(i) == '='
                        ) && (
                                anOperator(noSpaces.at(i + 1)) &&
                                noSpaces.at(i + 1) != '-'
                        )
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  invalid operator on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    syntaxError = "";
    return syntaxError;
}

string ImplicitGrapher::charToString(const char* const characters, const int& length) {
    string string;
    for (int i = 0; i < length; i++) {
        string += characters[i];
    }
    return string;
}

void ImplicitGrapher::processEquation() {
    bool debugEquation = false;
    for (int j = 0; j < maxEquationLength; j++) {
        constants[j] = 0;
    }

    char equation[2 * maxEquationLength];
    int length = memoryEquation.length();

    // Convert string to string array
    for (int j = 0; j < length; j++) {
        equation[j] = memoryEquation.at(j);
    }
    // Remove spaces and fill remainder with underscores
    if (debugEquation) {
        debug_string = "Remove spaces and fill remainder with underscores:";
    }
    valuesCounter = 0;
    for (int j = 0; j < length; valuesCounter++) {
        if (equation[valuesCounter] != ' ') {
            equation[j] = equation[valuesCounter];
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

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Isolate and convert '=' to '-'
    if (debugEquation) {
        debug_string += "Isolate and convert '=' to '-':";
    }
    equation[length + 3] = ')';
    valuesCounter = length - 1;
    while (equation[valuesCounter] != '=') {
        equation[valuesCounter + 3] = equation[valuesCounter];
        valuesCounter--;
    }
    equation[valuesCounter + 3] = '(';
    equation[valuesCounter + 2] = '-';
    equation[valuesCounter + 1] = ')';
    valuesCounter--;
    while (valuesCounter > -1) {
        equation[valuesCounter + 1] = equation[valuesCounter];
        valuesCounter--;
    }
    equation[0] = '(';
    length += 4;

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Add parentheses to ends
    if (debugEquation) {
        debug_string += "Add parentheses to ends:";
    }
    // Insert close parenthesis
    equation[length + 1] = ')';
    // Move characters
    for (int j = length; j > 0; j--) {
        equation[j] = equation[j - 1];
    }
    length += 2;
    // Insert open parenthesis
    equation[0] = '(';

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Add parentheses to functions
    if (debugEquation) {
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

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Add multiplication signs to close terms
    if (debugEquation) {
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

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Convert (-x to (0-x or (-1 to (0-1
    if (debugEquation) {
        debug_string += "Convert (-x to ((0-x) or (-1 to ((0-1):";
    }
    for (int j = 0; j < length; j++) {
        if (equation[j] == '(' && equation[j + 1] == '-') {
            // Move characters
            length++;
            for (int k = length - 1; k > j + 1; k--) {
                equation[k] = equation[k - 1];
            }
            // Insert zero
            equation[j + 1] = '0';
        }
    }

    if (debugEquation) {
        debug_string += charToString(equation, length);
    }

    // Add parentheses to symbol functions
    if (debugEquation) {
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
                    // Move characters
                    for (int k = length - 1; k > positions[0] + 1; k--) {
                        equation[k] = equation[k - 2];
                    }
                    // Insert close parenthesis
                    equation[positions[0] + 1] = ')';
                    // Move characters
                    for (int k = positions[0]; k > positions[1]; k--) {
                        equation[k] = equation[k - 1];
                    }
                    // Insert close parenthesis
                    equation[positions[1] + 1] = '(';
                }
            }
        }
    }

    if (debugEquation) {
        debug_string += charToString(equation, length) + ", ";
    }

    // Convert to coded equation
    if (debugEquation) {
        debug_string += "Convert to coded equation:";
    }
    int codedEquation[2 * maxEquationLength];
    int codedEquationCounter = 0;
    valuesCounter = 0;
    hasTimeVariable = false;
    for (int j = 0; j < length; j++, codedEquationCounter++) {
        if (equation[j] != '.' && !aDigit(equation[j])) {
            if (equation[j] == 'x') {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = X;
                valuesCounter++;
            }
            else if (equation[j] == 'y') {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = Y;
                valuesCounter++;
            }
            else if (equation[j] == 'z') {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = Z;
                valuesCounter++;
            }
            else if (equation[j] == 't' && equation[j + 1] != 'a') {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = T;
                valuesCounter++;
                hasTimeVariable = true;
            }
            else if (equation[j] == 'e') {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = E;
                valuesCounter++;
            }
            else if (equation[j] == pi) {
                codedEquation[codedEquationCounter] = constants[valuesCounter] = PI;
                valuesCounter++;
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
            codedEquation[codedEquationCounter] = valuesCounter;
            equationValues[valuesCounter] = values[valuesCounter] = number;
            valuesCounter++;
            j += numberSize - 1;
        }
    }
    // Display equation
    if (debugEquation) {
        debug_string += decode(codedEquation, codedEquationCounter, values);
    }

    // Convert 0-2 to -2
    // Convert 2-0 to 2
    // Convert 0+2 to 2
    // Convert 2+0 to 2
    // Convert 0*2 to 0
    // Convert 2*0 to 0
    // Convert 0/2 to 0
    // Convert 2/0 to error

    // Create Sequence Table destroying coded equation
    for (int j = 0, counter = 0; j < codedEquationCounter; j++) {
        if (codedEquation[j] > -1) {
            counter++;
        }
        else if (codedEquation[j] > -7) {
            codedEquation[j] = counter;
            counter++;
        }
    }

    bool end = false;
    for (sequenceLength = 0; !end; ) {
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
                            sequences[sequenceLength][0] = codedEquation[j - 2];//function
                            sequences[sequenceLength][1] = codedEquation[j - 1];//where first value is and where result will be stored
                            codedEquation[j - 3] = codedEquation[j - 1];
                            for (int k = j - 2; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 3];
                            }
                            sequenceLength++; }
                            break;
                        case 4: {// "(a+b)" "(a-b)" "(a*b)" "(a/b)" "(a^b)"
                            sequences[sequenceLength][0] = codedEquation[j - 2];//function
                            sequences[sequenceLength][1] = codedEquation[j - 3];//where first value is and where result will be stored
                            sequences[sequenceLength][2] = codedEquation[j - 1];//where second value is
                            codedEquation[j - 4] = codedEquation[j - 3];
                            for (int k = j - 3; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 4];
                            }
                            sequenceLength++; }
                            break;
                    }

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
float ImplicitGrapher::fOfXYZ(vec3 position) {
    position -= currentOffset;
    position *= zoom;
    for (int i = 0; i < valuesCounter; i++) {
        switch (constants[i]) {
            case 0: values[i] = equationValues[i]; break;
            case X: values[i] = position.x; break;
            case Y: values[i] = position.y; break;
            case Z: values[i] = position.z; break;
            case T: values[i] = t; break;
            case E: values[i] = M_E; break;
            case PI: values[i] = M_PI; break;
        }
    }
    float* v = values;
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
}

void ImplicitGrapher::calculateSurfaceOnCPU(float (*fOfXYZ)(vec3), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& clipEdges, PositionXYZNormalXYZ* _vertices, uvec3* _indices, GLuint& _numIndices) {
    ImplicitGrapher::zoom = zoom;
    ImplicitGrapher::t = timeVariable;
    t = timeVariable;
    currentOffset = defaultOffset + offset;
    // Erase normals
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].n = vec3(0.0f);
    }
    // Reset solution particleCount
    solutionCount = 0;
    // Reset group segment counter
    groupSegmentCounter = 0;
    // Calculate plusMinusAmounts and approximate solutions
    for (int i = 0; i < sizePlus3.x; i++) {
        for (int j = 0; j < sizePlus3.y; j++) {
            for (int k = 0; k < sizePlus3.z; k++) {
                plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k] = fOfXYZ(vec3(i, j, k)) > 0.0f;
                uvec3 ijk = uvec3(i, j, k);
                for (int l = 0; l < 3; l++) {
                    if (ijk[l] == 0u) {
                        continue;
                    }
                    uvec3 iCursor = ijk;
                    iCursor[l] -= 1u;
                    bool firstSample = plusMinus[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z];
                    bool secondSample = plusMinus[(ijk.x * sizePlus3.y + ijk.y) * sizePlus3.z + ijk.z];
                    // If no solution is detected jump to next iteration of loop.
                    if (firstSample == secondSample) {
                        continue;
                    }
                    xyzLineIndex[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z][l] = solutionCount;
                    int sign = secondSample ? POSITIVE : NEGATIVE;
                    for (int m = 0; m < 3; m++) {
                        _vertices[solutionCount].p[m] = iCursor[m];
                    }
                    float scan = 0.5f;
                    for (int m = 0; m < iterations; m++) {// Maybe use a do-while loop here to reduce the scan operations by 1.
                        _vertices[solutionCount].p[l] += scan;
                        if ((sign == POSITIVE) != (fOfXYZ(_vertices[solutionCount].p) > 0.0f)) {
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
                            combo = xYz * 8 | XYz * 4 | xyz * 2 | Xyz * 1;
                            break;
                        case 1://XY
                            combo = xYZ * 8 | XYZ * 4 | xyZ * 2 | XyZ * 1;
                            break;
                        case 2://YZ
                            combo = xyZ * 8 | xYZ * 4 | xyz * 2 | xYz * 1;
                            break;
                        case 3://YZ
                            combo = XyZ * 8 | XYZ * 4 | Xyz * 2 | XYz * 1;
                            break;
                        case 4://ZX
                            combo = Xyz * 8 | XyZ * 4 | xyz * 2 | xyZ * 1;
                            break;
                        case 5://ZX
                            combo = XYz * 8 | XYZ * 4 | xYz * 2 | xYZ * 1;
                            break;
                    }
                    switch (combo) {
                        case 0: break;//0000
                        case 1://0001
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 2://0010
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 3://0011
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 4://0100
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            break;
                        case 5://0101
                            if(l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 6: {//0110
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f) > 0.0f) {
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
                        case 7://0111
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 8://1000
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 9: {//1001
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f) > 0.0f) {
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
                        case 10://1010
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 11://1011
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 12://1100
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            break;
                        case 13://1101
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 14://1110
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            break;
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
                                sum += _vertices[groupSegments[_n][0]].p;
                                groupSegments[_n][2] = solutionCount;
                            }
                            _vertices[solutionCount].p = sum / segPerGroup;
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
            vec3 vectorA = _vertices[groupSegments[i][0]].p - _vertices[groupSegments[i][2]].p;
            vec3 vectorB = _vertices[groupSegments[i][1]].p - _vertices[groupSegments[i][2]].p;
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
        float length = vectorPointsPositive ? -distance(vec3(_vertices[i].n)) : distance(vec3(_vertices[i].n));
        _vertices[i].n /= length;
    }
    //Create TriangleView Primitives
    _numIndices = 0;
    int triangleCount = 0;
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1 && (!clipEdges || withinGraphRadius[i])) {
            _indices[triangleCount++] = groupSegments[i];
            _numIndices += 3;
        }
    }
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].p -= currentOffset;
    }
}

void ImplicitGrapher::calculateSurfaceOnCPU(std::function<float(vec3 _, NaiveSimulation& sim, const vec3& offset, const vec3& defaultOffset, const bool& sphereClipsGraph)> fOfXYZ, const float &timeVariable, const uint &iterations, const vec3 &offset, const float &zoom, const bool &clipEdges, PositionXYZNormalXYZ *_vertices, uvec3 *_indices, GLuint &_numIndices, NaiveSimulation& sim, const bool& sphereClipsGraph) {
    ImplicitGrapher::zoom = zoom;
    ImplicitGrapher::t = timeVariable;
    t = timeVariable;
    currentOffset = defaultOffset + offset;
    // Erase normals
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].n = vec3(0.0f);
    }
    // Reset solution particleCount
    solutionCount = 0;
    // Reset group segment counter
    groupSegmentCounter = 0;
    // Calculate plusMinusAmounts and approximate solutions
    for (int i = 0; i < sizePlus3.x; i++) {
        for (int j = 0; j < sizePlus3.y; j++) {
            for (int k = 0; k < sizePlus3.z; k++) {
                plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k] = fOfXYZ(vec3(i, j, k), sim, offset, defaultOffset, sphereClipsGraph) > 0.0f;
                uvec3 ijk = uvec3(i, j, k);
                for (int l = 0; l < 3; l++) {
                    if (ijk[l] == 0u) {
                        continue;
                    }
                    uvec3 iCursor = ijk;
                    iCursor[l] -= 1u;
                    bool firstSample = plusMinus[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z];
                    bool secondSample = plusMinus[(ijk.x * sizePlus3.y + ijk.y) * sizePlus3.z + ijk.z];
                    // If no solution is detected jump to next iteration of loop.
                    if (firstSample == secondSample) {
                        continue;
                    }
                    xyzLineIndex[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z][l] = solutionCount;
                    int sign = secondSample ? POSITIVE : NEGATIVE;
                    for (int m = 0; m < 3; m++) {
                        _vertices[solutionCount].p[m] = iCursor[m];
                    }
                    float scan = 0.5f;
                    for (int m = 0; m < iterations; m++) {// Maybe use a do-while loop here to reduce the scan operations by 1.
                        _vertices[solutionCount].p[l] += scan;
                        if ((sign == POSITIVE) != (fOfXYZ(_vertices[solutionCount].p, sim, offset, defaultOffset, sphereClipsGraph) > 0.0f)) {
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
                            combo = xYz * 8 | XYz * 4 | xyz * 2 | Xyz * 1;
                            break;
                        case 1://XY
                            combo = xYZ * 8 | XYZ * 4 | xyZ * 2 | XyZ * 1;
                            break;
                        case 2://YZ
                            combo = xyZ * 8 | xYZ * 4 | xyz * 2 | xYz * 1;
                            break;
                        case 3://YZ
                            combo = XyZ * 8 | XYZ * 4 | Xyz * 2 | XYz * 1;
                            break;
                        case 4://ZX
                            combo = Xyz * 8 | XyZ * 4 | xyz * 2 | xyZ * 1;
                            break;
                        case 5://ZX
                            combo = XYz * 8 | XYZ * 4 | xYz * 2 | xYZ * 1;
                            break;
                    }
                    switch (combo) {
                        case 0: break;//0000
                        case 1://0001
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 2://0010
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 3://0011
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 4://0100
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            break;
                        case 5://0101
                            if(l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 6: {//0110
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f, sim, offset, defaultOffset, sphereClipsGraph) > 0.0f) {
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
                        case 7://0111
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            break;
                        case 8://1000
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 9: {//1001
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ((_vertices[_node0].p + _vertices[_node1].p + _vertices[_node2].p + _vertices[_node3].p) * 0.25f, sim, offset, defaultOffset, sphereClipsGraph) > 0.0f) {
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
                        case 10://1010
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 11://1011
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            break;
                        case 12://1100
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            break;
                        case 13://1101
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            break;
                        case 14://1110
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            break;
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
                                sum += _vertices[groupSegments[_n][0]].p;
                                groupSegments[_n][2] = solutionCount;
                            }
                            _vertices[solutionCount].p = sum / segPerGroup;
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
            vec3 vectorA = _vertices[groupSegments[i][0]].p - _vertices[groupSegments[i][2]].p;
            vec3 vectorB = _vertices[groupSegments[i][1]].p - _vertices[groupSegments[i][2]].p;
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
        float length = vectorPointsPositive ? -distance(vec3(_vertices[i].n)) : distance(vec3(_vertices[i].n));
        _vertices[i].n /= length;
    }
    //Create TriangleView Primitives
    _numIndices = 0;
    int triangleCount = 0;
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1 && (!clipEdges || withinGraphRadius[i])) {
            _indices[triangleCount++] = groupSegments[i];
            _numIndices += 3;
        }
    }
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].p -= currentOffset;
    }
}

bool ImplicitGrapher::getPlusMinus(const int& i, const int& j, const int& k){
    return plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

inline ivec3 ImplicitGrapher::getXYZLineIndex(const int& i, const int& j, const int& k){
    return xyzLineIndex[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

void ImplicitGrapher::refactor(const ivec3& inputSize) {
    size = inputSize;
    sizePlus2 = size + ivec3(2);
    sizePlus3 = size + ivec3(3);
    defaultOffset = vec3(0.5f * inputSize + 1.0f);
    maxSolutionCount = 3 * sizePlus3.x * sizePlus3.y * sizePlus3.z;
}

size_t ImplicitGrapher::getRecommendedIndicesArraySize() {
    return 3 * maxSolutionCount * sizeof(uvec3);
}
