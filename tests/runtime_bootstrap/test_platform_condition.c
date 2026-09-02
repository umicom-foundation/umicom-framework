/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_platform_condition.c
 *
 * PURPOSE:
 *   Implement the test platform condition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    CHECK(umi_bootstrap_platform_condition_match("windows","windows"));
    CHECK(!umi_bootstrap_platform_condition_match("linux","windows"));
    return 0;
}
