/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_layout_normalizer.c
 *
 * PURPOSE:
 *   Focused regression coverage for ratio, order and geometry normalisation before cross-frontend layout comparison.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_normalizer.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    double v[3]={-1.0,0.5,2.0}; int32_t o[3]={3,1,2}; umi_fc_layout_normalizer_ratios(v,3U,0.0,1.0); umi_fc_layout_normalizer_orders(o,3U); CHECK(v[0]==0.0&&v[2]==1.0); CHECK(o[0]==1&&o[2]==3);
    return 0;
}
