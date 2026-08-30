/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_capability.c
 *
 * PURPOSE:
 *   Focused regression coverage for runtime capability bitsets used to gate application launches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_capability.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrRuntimeCapability c; umi_dr_runtime_capability_init(&c); c.required=UINT64_C(7); c.available=UINT64_C(3); CHECK(!umi_dr_runtime_capability_satisfied(&c)); c.available=7U; CHECK(umi_dr_runtime_capability_satisfied(&c));
    return 0;
}
