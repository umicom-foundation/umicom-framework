/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/mean_reversion.c
 *
 * PURPOSE:
 *   Calculate signed distance from a reference mean.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of mean reversion. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/mean_reversion.h"
double umi_mean_reversion_distance(double price,double reference){return reference!=0.0?price-reference:0.0;}
