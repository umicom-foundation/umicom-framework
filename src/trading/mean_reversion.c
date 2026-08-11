/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/mean_reversion.c
 *
 * PURPOSE:
 *   Calculate signed distance from a reference mean.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of mean reversion. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/mean_reversion.h"
double umi_mean_reversion_distance(double price,double reference){return reference!=0.0?price-reference:0.0;}
