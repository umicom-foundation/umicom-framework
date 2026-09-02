/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_breakpoint_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for validated ordered responsive breakpoints and viewport class resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/breakpoint_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcBreakpointProfile p; umi_fc_breakpoint_profile_init(&p); CHECK(umi_fc_breakpoint_profile_add(&p,800)==UMI_STATUS_OK); CHECK(umi_fc_breakpoint_profile_add(&p,1400)==UMI_STATUS_OK); CHECK(umi_fc_breakpoint_profile_bucket(&p,1200)==1U);
    return 0;
}
