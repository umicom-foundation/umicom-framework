/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_kit_json.c
 *
 * PURPOSE:
 *   Implement the test kit json behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | kit JSON output test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_json.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainKitSnapshot kit;
    UmiToolchainKitHealthSnapshot health;
    char json[8192];
    umi_toolchain_kit_init(&kit, "kit.json", "JSON kit");
    (void)strcpy(kit.profile_id, "profile");
    (void)strcpy(kit.target_triple, "x86_64-test");
    (void)strcpy(kit.c_compiler, "cc");
    kit.capabilities = UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
    kit.state = UMI_TOOLCHAIN_KIT_READY;
    assert(umi_toolchain_kit_health_evaluate(&kit,
        UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD, 0, &health) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_json_write(&kit, &health, NULL,
                                        json, sizeof(json)) == UMI_STATUS_OK);
    assert(strstr(json, "\"id\":\"kit.json\"") != NULL);
    assert(strstr(json, "\"health\"") != NULL);
    return 0;
}
