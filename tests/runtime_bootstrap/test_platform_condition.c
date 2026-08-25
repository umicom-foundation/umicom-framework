/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_platform_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Evaluate platform identifiers for conditional runtime composition.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/platform_condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    CHECK(umi_bootstrap_platform_condition_match("windows","windows"));
    CHECK(!umi_bootstrap_platform_condition_match("linux","windows"));
    return 0;
}
