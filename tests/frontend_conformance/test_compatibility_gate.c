/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_compatibility_gate.c
 *
 * PURPOSE:
 *   Focused regression coverage for application launch compatibility gate based on required semantic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/compatibility_gate.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcCompatibilityGate g={7U,0.8,true}; CHECK(umi_fc_compatibility_gate_evaluate(&g,7U,0.9)==UMI_FC_PASS); CHECK(umi_fc_compatibility_gate_evaluate(&g,3U,0.9)==UMI_FC_DEGRADED);
    return 0;
}
