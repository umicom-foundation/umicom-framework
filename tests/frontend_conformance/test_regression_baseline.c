/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_regression_baseline.c
 *
 * PURPOSE:
 *   Focused regression coverage for stored score and semantic fingerprint baseline for a frontend release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/regression_baseline.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRegressionBaseline b; CHECK(umi_fc_regression_baseline_make("gtk4",0.95,0U,123U,5U,&b)==UMI_STATUS_OK); CHECK(b.score==0.95&&b.revision==5U);
    return 0;
}
