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

#include <float.h>
#include <math.h>
#include <string.h>

/*
 * Provide the quant fx forward pricer value operation used by this module and its client
 * applications.
 */
/* The former unchecked calculation is retained for review. Validate inputs
 * and the staged result before publication. Apply exponent scaling when the
 * undiscounted intermediate cannot represent a normal number. */
#if 0
UmiStatus umi_quant_fx_forward_pricer_value(double notional,double contracted_forward,double market_forward,double discount_factor,double *out_pv){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_pv==NULL||notional<0.0||contracted_forward<=0.0||market_forward<=0.0||discount_factor<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_pv=notional*(market_forward-contracted_forward)*discount_factor;return UMI_STATUS_OK;}
#endif

UmiStatus umi_quant_fx_forward_pricer_value(double notional, double contracted_forward,
    double market_forward, double discount_factor, double *out_pv)
{
    if (out_pv == NULL || !isfinite(notional) || !isfinite(contracted_forward) ||
        !isfinite(market_forward) || !isfinite(discount_factor) || notional < 0.0 ||
        contracted_forward <= 0.0 || market_forward <= 0.0 || discount_factor < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const double spread = market_forward - contracted_forward;
    const double undiscounted = notional * spread;
    double value;
    if (isfinite(undiscounted) && fabs(undiscounted) >= DBL_MIN) {
        value = undiscounted * discount_factor;
    } else {
        /* Bounded fractions can be combined before restoring the exponent.
         * This handles both small and large intermediates without rounding
         * their value to zero or infinity before discounting. */
        int notional_exponent = 0, spread_exponent = 0, discount_exponent = 0;
        const double fraction = frexp(notional, &notional_exponent) * frexp(spread, &spread_exponent) *
            frexp(discount_factor, &discount_exponent);
        value = scalbn(fraction, notional_exponent + spread_exponent + discount_exponent);
    }
    if (!isfinite(value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out_pv = value;
    return UMI_STATUS_OK;
}
