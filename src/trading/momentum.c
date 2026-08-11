/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/momentum.c
 *
 * PURPOSE:
 *   Calculate simple close-to-close momentum.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of momentum. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/momentum.h"
double umi_price_momentum(double previous,double current){return previous>0.0?(current-previous)/previous:0.0;}
