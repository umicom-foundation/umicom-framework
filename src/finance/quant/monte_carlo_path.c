/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/monte_carlo_path.c
 *
 * PURPOSE:
 *   Generate one geometric-Brownian terminal path from an explicit normal draw.
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

#include "umicom/finance/quant/monte_carlo_path.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant monte carlo path terminal operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_monte_carlo_path_terminal(double spot,double drift,double volatility,double years,double normal_draw,double *out_terminal){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_terminal==NULL||spot<=0.0||volatility<0.0||years<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_terminal=spot*exp((drift-0.5*volatility*volatility)*years+volatility*sqrt(years)*normal_draw);return UMI_STATUS_OK;}
