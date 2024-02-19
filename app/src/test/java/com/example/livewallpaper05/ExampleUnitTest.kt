package com.example.livewallpaper05

import org.junit.Test

import org.junit.Assert.*
import org.junit.Before

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
class ExampleUnitTest {

    @Test
    fun checkEquationValid() {
        val eq = EquationChecker()
        val equation = "3 * (x^2 + 1) - y^2 = 8"
        val equation2 = "x^2 + y^2 + z^2 = 3"
        val result = eq.checkEquationSyntax(equation)
        val result2 = eq.checkEquationSyntax(equation2)
        assertEquals("", result)
        assertEquals("", result2)
    }

    @Test
    fun checkEquationMultipleEqualSign() {
        val eq = EquationChecker()
        val equation = "3 * (x+2) = 8 = y"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: contains more than one equal sign.", result)
    }

    @Test
    fun checkEquationMissingEqualSign() {
        val eq = EquationChecker()
        val equation = "2x + y"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: lacks an equal sign.", result)
    }

    @Test
    fun checkEquationImproperParenthesisOnLeft() {
        val eq = EquationChecker()
        val equation = "(3 * x + 2 = 8"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: invalid parenthesis count on left.", result)
    }

    @Test
    fun checkEquationImproperParenthesisOnRight() {
        val eq = EquationChecker()
        val equation = "3 * x + 2 = (8"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: invalid parenthesis count on right.", result)
    }


    @Test
    fun checkEquationNoExpressionOnRight() {
        val eq = EquationChecker()
        val equation = "x + y ="
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: no expression on right side.", result)
    }

    @Test
    fun checkEquationNoExpressionOnLeft() {
        val eq = EquationChecker()
        val equation = " = x^2 + y^2"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: no expression on left side.", result)
    }

    @Test
    fun checkEquationNoExpressionOnEither() {
        val eq = EquationChecker()
        val equation = " = "
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: no expression on either side.", result)
    }

    @Test
    fun checkEquationImproperDecimalOnRight() {
        val eq = EquationChecker()
        val equation = "2x = 6."
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: improper decimal on right side.", result)
    }

    @Test
    fun checkEquationImproperDecimalOnLeft(){
        val eq = EquationChecker()
        val equation = "y+2. = 6"
        val result = eq.checkEquationSyntax(equation)
        assertEquals("Syntax Error: improper decimal on left side.", result)
    }

}