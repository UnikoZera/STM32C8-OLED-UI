/*
 * calculator.c
 *
 *  Created on: May 16, 2025
 *      Author: UnikoZera
 */

#include "calculator.h"
#include <stddef.h> // For NULL if division by zero
#include <math.h>   // For sqrt and pow functions

// Function definitions
int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

int multiply(int a, int b)
{
    return a * b;
}

float divide(int a, int b)
{
    if (b == 0)
    {
        // Handle division by zero, e.g., return a specific value or set an error flag
        // For now, returning 0.0f and one might want to check for this case
        return 0.0f;
    }
    return (float)a / b;
}

// Function definition for square root
float square_root(float a)
{
    if (a < 0)
    {
        // Handle negative input for square root, e.g., return NaN or a specific error value
        // For now, returning 0.0f. NAN might be better: return NAN;
        return 0.0f;
    }
    return sqrtf(a);
}

// Function definition for power
double power(double base, double exp)
{
    return pow(base, exp);
}

// Function definition for solving ODE using Euler method
// dydx: pointer to the function defining the ODE dy/dx = f(x, y)
// x0: initial value of x
// y0: initial value of y (y at x0)
// x_target: the value of x at which to find y
// h: step size
float solve_ode_euler(float (*dydx)(float x, float y), float x0, float y0, float x_target, float h)
{
    float y = y0;
    float x = x0;

    // Iterate until x reaches x_target
    // Check if x_target is greater or smaller than x0 to determine iteration direction
    if (x_target > x0)
    {
        while (x < x_target)
        {
            if (x + h > x_target)
            {                     // If the next step overshoots
                h = x_target - x; // Adjust step size to hit x_target exactly
            }
            y = y + h * dydx(x, y); // Euler's method formula
            x = x + h;
        }
    }
    else if (x_target < x0)
    {
        while (x > x_target)
        {
            if (x - h < x_target)
            {                     // If the next step overshoots (for negative h direction)
                h = x - x_target; // Adjust step size to hit x_target exactly
            }
            // For iterating backwards, h should be negative, or dydx should be evaluated accordingly.
            // Assuming h is always positive and we adjust the formula for backward step.
            // Or, more simply, ensure h is negative if x_target < x0 when calling.
            // For this implementation, let's assume h is always positive and we are stepping towards x_target.
            // If h is meant to be the magnitude of step, then for x_target < x0, we'd subtract.
            // Let's refine this: the caller should provide a signed h, or we deduce it.
            // For simplicity, let's assume h is the step magnitude and we adjust based on direction.
            // This implementation assumes h is positive and we are iterating towards x_target.
            // If x_target < x0, the loop condition x > x_target is used.
            // We need to ensure h is treated as a step towards x_target.
            // A common way is to make h negative if x_target < x0 before starting.
            // However, the current loop structure handles positive h and adjusts the last step.
            // Let's stick to positive h and adjust iteration.
            // If we are going from x0 to a smaller x_target, h should effectively be negative.
            // The provided h is a step *size*, so it's positive. We adjust the formula for stepping backward.
            y = y - h * dydx(x, y); // Euler's method for stepping backward (x decreases)
            x = x - h;
        }
    }
    // At this point, x should be approximately x_target, and y is the solution at x_target.
    return y;
}

// Function definition for solving double integral using Trapezoidal rule
// func: pointer to the function f(x,y) to be integrated
// x_min, x_max: integration limits for x
// y_min, y_max: integration limits for y (must be constant for this implementation)
// nx, ny: number of subintervals for x and y dimensions, respectively
double solve_double_integral_trapezoidal(
    double (*func)(double x, double y),
    double x_min, double x_max,
    double y_min, double y_max,
    int nx, int ny)
{
    double hx = (x_max - x_min) / nx; // Step size for x
    double hy = (y_max - y_min) / ny; // Step size for y
    double integral_sum = 0.0;

    // Apply the composite Trapezoidal rule for double integrals
    // Sum over the interior points with full weight
    for (int i = 1; i < nx; i++)
    {
        for (int j = 1; j < ny; j++)
        {
            integral_sum += func(x_min + i * hx, y_min + j * hy);
        }
    }

    // Sum over the edges (excluding corners) with weight 1/2
    for (int i = 1; i < nx; i++)
    {
        integral_sum += 0.5 * (func(x_min + i * hx, y_min) + func(x_min + i * hx, y_max));
    }
    for (int j = 1; j < ny; j++)
    {
        integral_sum += 0.5 * (func(x_min, y_min + j * hy) + func(x_max, y_min + j * hy));
    }

    // Add the corners with weight 1/4, then multiply by hx*hy
    // A more standard way to write the composite trapezoidal rule for 2D is:
    // I = (hx*hy/4) * [ f(x0,y0) + f(xN,y0) + f(x0,yM) + f(xN,yM) +
    //                   2 * (sum_edges_not_corners) +
    //                   4 * (sum_interior_points) ]
    // However, the previous summation approach is also common. Let's adjust to a clearer form.

    integral_sum = 0.0; // Reset sum for the standard formula structure

    // Sum f(xi, yj) with appropriate weights
    for (int i = 0; i <= nx; i++)
    {
        for (int j = 0; j <= ny; j++)
        {
            double x = x_min + i * hx;
            double y = y_min + j * hy;
            double term = func(x, y);

            if (i == 0 || i == nx)
            { // Points on x_min or x_max boundary
                term *= 0.5;
            }
            if (j == 0 || j == ny)
            { // Points on y_min or y_max boundary
                term *= 0.5;
            }
            integral_sum += term;
        }
    }

    return integral_sum * hx * hy;
}
