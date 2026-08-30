/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_checksum_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for checksum algorithm and digest presence/match requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/checksum_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrChecksumPolicy p; umi_dr_checksum_policy_init(&p); CHECK(umi_dr_checksum_policy_accept(&p,"abc","abc")); CHECK(!umi_dr_checksum_policy_accept(&p,"abc","def"));
    return 0;
}
