/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_coverage_summary.c
 *
 * PURPOSE:
 *   Focused regression coverage for required/optional semantic capability coverage summary for a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/coverage_summary.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcCoverageSummary s; umi_fc_coverage_summary_calculate(3U,4U,3U,&s); CHECK(s.required_score==1.0); CHECK(s.total_score>0.66);
    return 0;
}
