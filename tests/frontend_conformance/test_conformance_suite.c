/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_conformance_suite.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded ordered collection of conformance rules for one application or workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_suite.h"
#include "umicom/frontend/conformance/conformance_rule.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcConformanceSuite s; UmiFcConformanceRule r; umi_fc_conformance_suite_init(&s,"studio"); CHECK(umi_fc_conformance_rule_make("dock",UMI_FC_BLOCKER,1U,1.0,&r)==UMI_STATUS_OK); CHECK(umi_fc_conformance_suite_add(&s,&r)==UMI_STATUS_OK); CHECK(umi_fc_conformance_suite_blocker_count(&s)==1U);
    return 0;
}
