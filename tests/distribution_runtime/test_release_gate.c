/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_release_gate.c
 *
 * PURPOSE:
 *   Focused regression coverage for release decision combining signature, checksum, compatibility and test evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_gate.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrReleaseGateInput i={true,true,true,true,true,0U}; CHECK(umi_dr_release_gate_pass(&i)); i.blockers=1U; CHECK(!umi_dr_release_gate_pass(&i));
    return 0;
}
