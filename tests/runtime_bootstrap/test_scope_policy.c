/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_scope_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for Enforce legal lifetime relationships between owners and requested dependencies.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/scope_policy.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    CHECK(umi_bootstrap_scope_policy_can_depend(UMI_BOOTSTRAP_SCOPE_REQUEST, UMI_BOOTSTRAP_SCOPE_SINGLETON));
    CHECK(!umi_bootstrap_scope_policy_can_depend(UMI_BOOTSTRAP_SCOPE_SINGLETON, UMI_BOOTSTRAP_SCOPE_REQUEST));
    CHECK(umi_bootstrap_scope_policy_can_depend(UMI_BOOTSTRAP_SCOPE_SINGLETON, UMI_BOOTSTRAP_SCOPE_TRANSIENT));
    return 0;
}
