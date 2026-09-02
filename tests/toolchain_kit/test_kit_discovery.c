/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_kit/test_kit_discovery.c
 *
 * PURPOSE:
 *   Implement the test kit discovery behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | kit discovery composition test | Sammy Hegab | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/kit_discovery.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainCatalogue *toolchains = NULL;
    UmiToolchainKitCatalogue *kits = NULL;
    UmiToolchainProfile profile;
    UmiToolchainKitDiscoveryRequest request;
    UmiToolchainKitDiscoverySnapshot snapshot;
    assert(umi_toolchain_catalogue_create(&toolchains) == UMI_STATUS_OK);
    assert(umi_toolchain_kit_catalogue_create(&kits) == UMI_STATUS_OK);
    umi_toolchain_profile_init(&profile);
    (void)strcpy(profile.profile_id, "synthetic-gcc");
    (void)strcpy(profile.display_name, "Synthetic GCC");
    (void)strcpy(profile.target_triple, "x86_64-synthetic");
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    (void)strcpy(profile.tools[UMI_TOOL_GCC].path, "/synthetic/gcc");
    profile.complete = 0;
    assert(umi_toolchain_catalogue_upsert_profile(toolchains, &profile) == UMI_STATUS_OK);
    umi_toolchain_kit_discovery_request_init(&request);
    request.host_triple = "x86_64-synthetic";
    request.include_incomplete_profiles = 1;
    request.discover_python = 0;
    assert(umi_toolchain_kit_discover_catalogue(toolchains, kits,
                                                &request, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.attempted_profiles == 1U);
    assert(snapshot.kits_created == 1U);
    assert(snapshot.degraded_kits == 1U);
    umi_toolchain_kit_catalogue_destroy(kits);
    umi_toolchain_catalogue_destroy(toolchains);
    return 0;
}
