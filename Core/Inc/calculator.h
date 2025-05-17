/*
 * calculator.h
 *
 *  Created on: May 16, 2025
 *      Author: UnikoZera
 */

#ifndef INC_CALCULATOR_H_
#define INC_CALCULATOR_H_

// Function declarations
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
float divide(int a, int b);

// New function declarations for square root and power
float square_root(float a);
double power(double base, double exp);

// Function declaration for solving ODE using Euler method
// dydx: pointer to the function defining the ODE dy/dx = f(x, y)
// x0: initial value of x
// y0: initial value of y (y at x0)
// x_target: the value of x at which to find y
// h: step size
float solve_ode_euler(float (*dydx)(float x, float y), float x0, float y0, float x_target, float h); // Function declaration for solving ODE using Euler method

// Function declaration for solving double integral using Trapezoidal rule
// func: pointer to the function f(x,y) to be integrated
// x_min, x_max: integration limits for x
// y_min, y_max: integration limits for y (must be constant for this implementation)
// nx, ny: number of subintervals for x and y dimensions, respectively
double solve_double_integral_trapezoidal(
    double (*func)(double x, double y),
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny);

#endif /* INC_CALCULATOR_H_ */
