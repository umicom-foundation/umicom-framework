/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_selection_publication.c
 *
 * PURPOSE:
 *   Verify complete toolchain selection and receiving environment pointer ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/toolchain/catalogue.h"
static UmiToolchainProfile profile, selected, previous_profile;
static UmiEnvironmentPlan environment, previous_environment;
static UmiToolchainCatalogueSnapshot before, after;
static void Profile(const char *id,const char *compiler)
{
    umi_toolchain_profile_init(&profile);
    (void)strcpy(profile.profile_id,id);
    (void)strcpy(profile.display_name,id);
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.tools[UMI_TOOL_GCC].state = UMI_TOOL_VALIDATED;
    (void)strcpy(profile.tools[UMI_TOOL_GCC].path,compiler);
}
int main(void)
{
    UmiToolchainCatalogue *catalogue = NULL;
    assert(umi_toolchain_catalogue_create(&catalogue) == UMI_STATUS_OK);
    Profile("good","gcc-fixture");
    assert(umi_toolchain_catalogue_upsert_profile(catalogue,&profile) == UMI_STATUS_OK);
    Profile("bad","gcc\nrefused");
    assert(umi_toolchain_catalogue_upsert_profile(catalogue,&profile) == UMI_STATUS_OK);
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,"good",&selected,&environment) == UMI_STATUS_OK);
    assert(strcmp(selected.profile_id,"good") == 0);
    assert(strcmp(umi_environment_plan_find(&environment,"CC"),"gcc-fixture") == 0);
    for (size_t i = 0U; i < environment.count; ++i) {
        assert(environment.process_entries[i].name == environment.entries[i].name);
        assert(environment.process_entries[i].value == environment.entries[i].value);
    }
    (void)memcpy(&previous_profile,&selected,sizeof(selected));
    (void)memcpy(&previous_environment,&environment,sizeof(environment));
    assert(umi_toolchain_catalogue_snapshot(catalogue,&before) == UMI_STATUS_OK);
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,"bad",&selected,&environment) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&previous_profile,&selected,sizeof(selected)) == 0);
    assert(memcmp(&previous_environment,&environment,sizeof(environment)) == 0);
    assert(umi_toolchain_catalogue_snapshot(catalogue,&after) == UMI_STATUS_OK);
    assert(before.revision == after.revision && strcmp(after.selected_profile_id,"good") == 0);
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,"missing",&selected,&environment) == UMI_STATUS_NOT_FOUND);
    assert(memcmp(&previous_profile,&selected,sizeof(selected)) == 0);
    assert(memcmp(&previous_environment,&environment,sizeof(environment)) == 0);
    /* The identifier can borrow receiving profile text; staging protects it. */
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,selected.profile_id,&selected,&environment) == UMI_STATUS_OK);
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,"good",&selected,
        (UmiEnvironmentPlan *)(void *)&selected) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_toolchain_catalogue_select_with_environment(catalogue,"good",
        (UmiToolchainProfile *)(void *)catalogue,&environment) == UMI_STATUS_INVALID_ARGUMENT);
    Profile("malformed","gcc");
    (void)memset(profile.profile_id,'x',sizeof(profile.profile_id));
    assert(!umi_toolchain_profile_storage_valid(&profile));
    assert(umi_toolchain_catalogue_upsert_profile(catalogue,&profile) == UMI_STATUS_INVALID_ARGUMENT);
    Profile("malformed","gcc"); profile.structure_size = 0U;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    assert(umi_toolchain_catalogue_upsert_profile(catalogue,&profile) == UMI_STATUS_INVALID_ARGUMENT);
    Profile("malformed","gcc"); profile.tools[UMI_TOOL_GCC].kind = UMI_TOOL_CLANG;
    assert(!umi_toolchain_profile_storage_valid(&profile));
    assert(umi_toolchain_catalogue_upsert_profile(catalogue,&profile) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_toolchain_catalogue_profile_count(catalogue) == 2U);
    UmiToolchainProjectBindingSnapshot binding = {0};
    (void)strcpy(binding.project_id,"project");
    (void)strcpy(binding.profile_id,"good");
    assert(umi_toolchain_catalogue_bind_project(catalogue,&binding) == UMI_STATUS_OK);
    assert(umi_toolchain_catalogue_snapshot(catalogue,&before) == UMI_STATUS_OK);
    (void)memset(binding.profile_id,'x',sizeof(binding.profile_id));
    assert(umi_toolchain_catalogue_bind_project(catalogue,&binding) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_toolchain_catalogue_snapshot(catalogue,&after) == UMI_STATUS_OK);
    assert(before.revision == after.revision && after.project_binding_count == 1U);
    UmiToolchainCatalogueDiscoveryRequest discovery = {0};
    UmiToolchainCatalogueDiscoverySnapshot discovery_result;
    discovery.explicit_root_count = 1U;
    assert(umi_toolchain_catalogue_discover(catalogue,&discovery,&discovery_result) == UMI_STATUS_INVALID_ARGUMENT);
    umi_toolchain_catalogue_destroy(catalogue);
    return 0;
}
