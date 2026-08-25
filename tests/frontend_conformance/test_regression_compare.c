/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_regression_compare.c
 *
 * PURPOSE:
 *   Focused regression coverage for score, blocker and fingerprint regression detection against a baseline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/regression_compare.h"
#include "umicom/frontend/conformance/regression_baseline.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRegressionBaseline b={"web",0.95,0U,10U,1U}; UmiFcRegressionComparison c; umi_fc_regression_compare_run(&b,0.90,0U,11U,0.02,&c); CHECK(c.regressed); CHECK(c.fingerprint_changed);
    return 0;
}
