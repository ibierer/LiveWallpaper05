package com.xendovo

class EquationChecker {
    fun checkEquationSyntax(editable: String): String {
        var syntaxError = ""
        var containsEqualSign = false
        var parenthesisCountLeft = 0
        var parenthesisCountRight = 0
        var improperParenthesisLeft = false
        var improperParenthesisRight = false

        for (i in editable.indices) {
            when (editable[i]) {
                '(' -> if (containsEqualSign) parenthesisCountRight++ else parenthesisCountLeft++
                ')' -> if (containsEqualSign) parenthesisCountRight-- else parenthesisCountLeft--
                '=' -> if (containsEqualSign) {
                    syntaxError = "Syntax Error: contains more than one equal sign."
                    return syntaxError
                } else {
                    containsEqualSign = true
                }
            }

            if (parenthesisCountLeft < 0) {
                improperParenthesisLeft = true
            } else if (parenthesisCountRight < 0) {
                improperParenthesisRight = true
            }
        }

        if (!containsEqualSign) {
            syntaxError = "Syntax Error: lacks an equal sign."
            return syntaxError
        }

        if (improperParenthesisLeft || improperParenthesisRight) {
            syntaxError = "Syntax Error: improper parenthesis on "
            syntaxError += if (!improperParenthesisLeft) {
                "right."
            } else if (improperParenthesisRight) {
                "both sides."
            } else {
                "left."
            }
            return syntaxError
        }

        if (parenthesisCountLeft != 0 || parenthesisCountRight != 0) {
            syntaxError = "Syntax Error: invalid parenthesis count on "
            syntaxError += if (parenthesisCountLeft == 0) {
                "right."
            } else if (parenthesisCountRight != 0) {
                "both sides."
            } else {
                "left."
            }
            return syntaxError
        }

        // Check for expressions
        var termOnLeft = false
        var termOnRight = false
        var passedEqualSign = false
        for (i in editable.indices) {
            if (aDigit(editable[i]) || aCharacter(editable[i])) {
                if (passedEqualSign) {
                    termOnRight = true
                } else {
                    termOnLeft = true
                }
            } else if (editable[i] == '=') {
                passedEqualSign = true
            }
        }

        if (!termOnLeft || !termOnRight) {
            syntaxError = "Syntax Error: no expression on "
            syntaxError += if (termOnLeft) {
                "right side."
            } else if (!termOnRight) {
                "either side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Improper term I
        var badTermOnLeft = false
        var badTermOnRight = false
        passedEqualSign = false
        for (i in 0 until editable.length - 1) {
            if (aCharacter(editable[i]) &&
                (editable[i + 1] == '.' || aDigit(editable[i + 1]))
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            } else if (editable[i] == '=') {
                passedEqualSign = true
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper term on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Convert to noSpaces
        val noSpaces = editable.filter { it != ' ' }

        // Improper term II
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((noSpaces[i] == '(' &&
                        (noSpaces[i + 1] == ')' ||
                                (anOperator(noSpaces[i + 1]) && noSpaces[i + 1] != '-')
                                )
                        ) || (
                        (noSpaces[i] == '(' || anOperator(noSpaces[i])) &&
                                noSpaces[i + 1] == ')'
                        )
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper term on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Improper Decimal
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((noSpaces[i] == '.' && !aDigit(noSpaces[i + 1])) ||
                (i == noSpaces.length - 2 && noSpaces[noSpaces.length - 1] == '.')
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper decimal on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Invalid Operator
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((i == noSpaces.length - 2 && anOperator(noSpaces[noSpaces.length - 1])) ||
                (anOperator(noSpaces[i]) &&
                        (noSpaces[i + 1] == ')' || noSpaces[i + 1] == '=' || (i == 0 && noSpaces[i] != '-'))
                        ) || (
                        (noSpaces[i] == '(' || noSpaces[i] == '=') &&
                                (anOperator(noSpaces[i + 1]) && noSpaces[i + 1] != '-')
                        )
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }
        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: invalid operator on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            }
            else {
                syntaxError += "left side."
            }
            return syntaxError
        }
        syntaxError = ""
        return syntaxError
    }
    private fun aDigit(character: Char): Boolean {
        return character in '0'..'9'
    }
    private fun aCharacter(character: Char): Boolean {
        return character == 'x' ||
                character == 'y' ||
                character == 'z' ||
                character == 't' ||
                character == 'e' ||
                character == pi
    }

    private fun anOperator(character: Char): Boolean {
        return character == '^' ||
                character == '/' ||
                character == '*' ||
                character == '-' ||
                character == '+'
    }

    private val pi: Char = 5.toChar()
}