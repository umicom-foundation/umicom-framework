/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_lifetime_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for Centralise eager, lazy and externally-owned service creation policy.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/lifetime_policy.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    CHECK(umi_bootstrap_lifetime_should_create_eagerly(UMI_BOOTSTRAP_LIFETIME_EAGER,false));
    CHECK(!umi_bootstrap_lifetime_should_create_eagerly(UMI_BOOTSTRAP_LIFETIME_LAZY,false));
    CHECK(umi_bootstrap_lifetime_should_create_eagerly(UMI_BOOTSTRAP_LIFETIME_LAZY,true));
    CHECK(!umi_bootstrap_lifetime_framework_owns(UMI_BOOTSTRAP_LIFETIME_EXTERNAL));
    return 0;
}
