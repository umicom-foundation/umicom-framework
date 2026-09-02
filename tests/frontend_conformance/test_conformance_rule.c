/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_conformance_rule.c
 *
 * PURPOSE:
 *   Focused regression coverage for weighted conformance rules with severity and required capability evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_rule.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcConformanceRule r; CHECK(umi_fc_conformance_rule_make("dock",UMI_FC_BLOCKER,3U,2.0,&r)==UMI_STATUS_OK); CHECK(umi_fc_conformance_rule_passes(&r,7U)); CHECK(!umi_fc_conformance_rule_passes(&r,1U));
    return 0;
}
