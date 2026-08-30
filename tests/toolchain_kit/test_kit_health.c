/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_kit_health.c
 *
 * PURPOSE:
 *   Implement the test kit health behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | kit health test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_health.h"

int main(void)
{
    UmiToolchainKitSnapshot kit;
    UmiToolchainKitHealthSnapshot health;
    umi_toolchain_kit_init(&kit, "kit.health", "Health kit");
    (void)strcpy(kit.profile_id, "profile");
    (void)strcpy(kit.target_triple, "x86_64-test");
    (void)strcpy(kit.c_compiler, "cc");
    kit.capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
    kit.state = UMI_TOOLCHAIN_KIT_READY;
    assert(umi_toolchain_kit_health_evaluate(&kit,
        UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD, 0, &health) == UMI_STATUS_OK);
    assert(health.state == UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY);
    assert(health.ready == 1);
    assert(umi_toolchain_kit_health_evaluate(&kit,
        UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD |
        UMI_TOOLCHAIN_KIT_CAPABILITY_DEBUG, 0, &health) == UMI_STATUS_OK);
    assert(health.state == UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED);
    assert((health.issue_flags & UMI_TOOLCHAIN_KIT_HEALTH_MISSING_DEBUGGER) != 0U);
    return 0;
}
