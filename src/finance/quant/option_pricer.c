/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/option_pricer.c
 *
 * PURPOSE:
 *   Provide intrinsic and time-value decomposition for vanilla options.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/option_pricer.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant option pricer intrinsic operation used by this module and its client
 * applications.
 */
double umi_quant_option_pricer_intrinsic(double spot,double strike,UmiQuantOptionRight right){return fmax((double)right*(spot-strike),0.0);}
/*
 * Provide the quant option pricer time value operation used by this module and its client
 * applications.
 */
double umi_quant_option_pricer_time_value(double total_value,double spot,double strike,UmiQuantOptionRight right){return fmax(total_value-umi_quant_option_pricer_intrinsic(spot,strike,right),0.0);}
