/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/fx_forward_pricer.c
 *
 * PURPOSE:
 *   Price FX forwards from contracted and market forward rates.
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

#include "umicom/finance/quant/fx_forward_pricer.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant fx forward pricer value operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_fx_forward_pricer_value(double notional,double contracted_forward,double market_forward,double discount_factor,double *out_pv){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_pv==NULL||notional<0.0||contracted_forward<=0.0||market_forward<=0.0||discount_factor<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_pv=notional*(market_forward-contracted_forward)*discount_factor;return UMI_STATUS_OK;}
