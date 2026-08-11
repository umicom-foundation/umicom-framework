/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/factor_observation.c
 *
 * PURPOSE:
 *   Calculate the change of an observed market factor around a movement event.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of factor observation. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/factor_observation.h"
double umi_factor_change(const UmiMarketFactor *f){return f!=NULL?f->value_after-f->value_before:0.0;}
