/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/factor_attribution.c
 *
 * PURPOSE:
 *   Calculate simple signed factor contribution from change and weight.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of factor attribution. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/factor_attribution.h"
/*
 * Provide the factor contribution operation used by this module and its client
 * applications.
 */
double umi_factor_contribution(double change,double weight){return change*weight;}
