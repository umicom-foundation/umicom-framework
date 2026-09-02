/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_release_gate.c
 *
 * PURPOSE:
 *   Focused regression coverage for release-readiness gate combining parity, regression, accessibility and blocker evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/release_gate.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcReleaseGate g={0.92,0.95,0.02,true}; CHECK(umi_fc_release_gate_accepts(&g,0.96,0.98,0.01,0U)); CHECK(!umi_fc_release_gate_accepts(&g,1.0,1.0,0.0,1U));
    return 0;
}
