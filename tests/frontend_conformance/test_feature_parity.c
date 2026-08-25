/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_feature_parity.c
 *
 * PURPOSE:
 *   Focused regression coverage for missing, extra and shared capability calculation between canonical and candidate renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/feature_parity.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcFeatureParity p; CHECK(umi_fc_feature_parity_calculate(7U,3U,&p)==UMI_STATUS_OK); CHECK(p.missing==4U); CHECK(p.score>0.66&&p.score<0.67);
    return 0;
}
