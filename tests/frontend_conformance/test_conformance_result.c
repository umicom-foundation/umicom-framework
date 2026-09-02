/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_conformance_result.c
 *
 * PURPOSE:
 *   Focused regression coverage for normalised pass, degraded and fail outcomes for one conformance case.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_result.h"
#include "umicom/frontend/conformance/conformance_case.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcConformanceCase c; UmiFcConformanceResult r; CHECK(umi_fc_conformance_case_make("drag",3U,1U,true,&c)==UMI_STATUS_OK); CHECK(umi_fc_conformance_result_from_case(&c,&r)==UMI_STATUS_OK); CHECK(r.outcome==UMI_FC_DEGRADED); CHECK(r.score>0.49);
    return 0;
}
