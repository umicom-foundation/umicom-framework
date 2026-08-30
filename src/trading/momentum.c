/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/momentum.c
 *
 * PURPOSE:
 *   Calculate simple close-to-close momentum.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of momentum. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/momentum.h"
double umi_price_momentum(double previous,double current){return previous>0.0?(current-previous)/previous:0.0;}
