/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_pricing_engine.c
 *
 * PURPOSE:
 *   Verify deterministic FX-forward, discount and present-value calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include "umicom/open_tms/pricing_engine.h"

int main(void)
{
    double forward = 0.0, discount = 0.0, pv = 0.0;
    assert(umi_open_tms_price_fx_forward(
               1.25, 0.05, 0.03, 0.5, &forward) == UMI_STATUS_OK);
    assert(forward > 1.26 && forward < 1.27);
    assert(umi_open_tms_discount_factor(0.05, 0.5, &discount) == UMI_STATUS_OK);
    assert(discount > 0.97 && discount < 0.98);
    assert(umi_open_tms_present_value(1000.0, discount, &pv) == UMI_STATUS_OK);
    assert(pv > 970.0 && pv < 980.0);
    return 0;
}
