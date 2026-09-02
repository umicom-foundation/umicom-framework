/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/swap_pricer.c
 *
 * PURPOSE:
 *   Price fixed-versus-floating swaps from a par-rate difference and annuity.
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

#include "umicom/finance/quant/swap_pricer.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant swap pricer value operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_swap_pricer_value(double notional,double annuity,double fixed_rate,double par_rate,double *out_pv){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_pv==NULL||notional<0.0||annuity<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_pv=notional*annuity*(par_rate-fixed_rate);return UMI_STATUS_OK;}
