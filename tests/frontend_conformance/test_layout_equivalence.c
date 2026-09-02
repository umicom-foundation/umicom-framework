/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_layout_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for structural and geometric layout-equivalence scoring with configurable tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    double a[3]={0.2,0.5,0.8},b[3]={0.21,0.49,0.95}; double s=umi_fc_layout_equivalence_values(a,b,3U,0.03); CHECK(s>0.66&&s<0.67); CHECK(!umi_fc_layout_equivalence_passes(s,0.9));
    return 0;
}
