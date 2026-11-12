/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_toolchain_sync.c
 *
 * PURPOSE:
 *   Verify selected driver and target replacement without stale profiles or partial synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "compiler_centre_fixture.h"
static UmiToolchainProfile toolchain;
static UmiCompilerPlan plan;
int main(void)
{
    UmiCompilerCentre *centre = malloc(sizeof(*centre)), *saved = malloc(sizeof(*saved));
    CENTRE_REQUIRE(centre != NULL && saved != NULL);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    umi_toolchain_profile_init(&toolchain);
    (void)strcpy(toolchain.display_name,"Selection");
    (void)strcpy(toolchain.target_triple,"x86_64-unknown-linux-gnu");
    toolchain.selected_c_compiler = UMI_TOOL_GCC;
    toolchain.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    (void)strcpy(toolchain.tools[UMI_TOOL_GCC].path,"first-gcc");
    CENTRE_REQUIRE(umi_compiler_centre_sync_toolchain(centre,&toolchain,"selected") == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->profile_count == 2U && centre->providers.count == 1U);
    (void)strcpy(toolchain.tools[UMI_TOOL_GCC].path,"second-gcc");
    (void)strcpy(toolchain.target_triple,"aarch64-unknown-linux-gnu");
    CENTRE_REQUIRE(umi_compiler_centre_sync_toolchain(centre,&toolchain,"selected") == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->profile_count == 2U && centre->providers.count == 1U);
    const UmiCompilerProfile *active = umi_compiler_centre_active_profile(centre);
    CENTRE_REQUIRE(active != NULL && strcmp(active->target.triple,toolchain.target_triple) == 0);
    CENTRE_REQUIRE(strcmp(active->linker.driver,"second-gcc") == 0);
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit_in_directory(centre,"project","main","explicit-project",&plan) == UMI_STATUS_OK);
    CENTRE_REQUIRE(strcmp(plan.executable,"second-gcc") == 0 && strcmp(plan.working_directory,"explicit-project") == 0);
    toolchain.selected_c_compiler = UMI_TOOL_CLANG;
    toolchain.tools[UMI_TOOL_CLANG].state = UMI_TOOL_FOUND;
    (void)strcpy(toolchain.tools[UMI_TOOL_CLANG].path,"selected-clang");
    CENTRE_REQUIRE(umi_compiler_centre_sync_toolchain(centre,&toolchain,"selected") == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->profile_count == 2U && centre->providers.count == 2U);
    active = umi_compiler_centre_active_profile(centre);
    CENTRE_REQUIRE(active != NULL && strcmp(active->provider_id,"clang") == 0);
    CENTRE_REQUIRE(umi_compiler_registry_find(&centre->providers,"gcc") != NULL);
    /* Unrelated collections and the last command remain until explicitly replanned. */
    CENTRE_REQUIRE(centre->manifest_count == 1U && centre->compilation_database.count == 1U);
    CENTRE_REQUIRE(strcmp(centre->compilation_database.items[0].plan.executable,"second-gcc") == 0);
    centre->providers.revision = UINT64_MAX;
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_sync_toolchain(centre,&toolchain,"selected") == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    free(saved); free(centre);
    return 0;
}
