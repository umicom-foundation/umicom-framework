/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_signature_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for signature requirement and trusted-publisher policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/signature_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrSignaturePolicy p; umi_dr_signature_policy_init(&p); CHECK(!umi_dr_signature_policy_accept(&p,false,true,true)); CHECK(umi_dr_signature_policy_accept(&p,true,true,false));
    return 0;
}
