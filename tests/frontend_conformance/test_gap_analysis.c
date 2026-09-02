/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_gap_analysis.c
 *
 * PURPOSE:
 *   Focused regression coverage for required, optional and quality gap analysis between canonical surface and renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/gap_analysis.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcGapAnalysis g; umi_fc_gap_analysis_run(3U,4U,3U,&g); CHECK(g.required_count==0U); CHECK(g.optional_count==1U); CHECK(g.coverage>0.66);
    return 0;
}
