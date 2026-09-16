/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/finite_numbers/main.c
 *
 * PURPOSE:
 *   Floating-point values and finite input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static bool PositiveProduct(double left, double right, double *out)
{
    if (out==NULL || !isfinite(left) || !isfinite(right) || left<=0.0 || right<=0.0)
        return false;
    if (left>1.0 && right>DBL_MAX/left) return false;
    double product=left*right;
    if (!isfinite(product) || product<=0.0) return false;
    *out=product; return true;
}
int main(void)
{
    double product=9.0;
    if (PositiveProduct(NAN,1.0,&product) || PositiveProduct(INFINITY,1.0,&product) ||
        PositiveProduct(DBL_MAX,2.0,&product) || product!=9.0) return EXIT_FAILURE;
    if (!PositiveProduct(1.5,2.0,&product) || product!=3.0) return EXIT_FAILURE;
    /* A scale-2 cash example uses integer minor units, not a trading recommendation. */
    const long cashMinor=12345L, feeMinor=25L;
    if (cashMinor-feeMinor!=12320L) return EXIT_FAILURE;
    puts("finite product=3; overflow/NaN/infinity rejected; minor units=12320");
    puts("PASS: finite_numbers");
    return EXIT_SUCCESS;
}
