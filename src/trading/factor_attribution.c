/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/factor_attribution.c
 *
 * PURPOSE:
 *   Calculate simple signed factor contribution from change and weight.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of factor attribution. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/factor_attribution.h"
double umi_factor_contribution(double change,double weight){return change*weight;}
