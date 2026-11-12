/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_centre_admission.c
 *
 * PURPOSE:
 *   Reject malformed nested compiler-centre owners and preserve checked snapshots and mutations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "compiler_centre_fixture.h"
static UmiCompilerPlan plan, plan_before;
int main(void)
{
    UmiCompilerCentre *centre = malloc(sizeof(*centre)), *before = malloc(sizeof(*before));
    CENTRE_REQUIRE(centre != NULL && before != NULL);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    CENTRE_REQUIRE(umi_compiler_centre_storage_valid(centre));
    UmiCompilerCentreSnapshot snapshot = {0}, snapshot_before;
    CENTRE_REQUIRE(umi_compiler_centre_snapshot_checked(centre,&snapshot) == UMI_STATUS_OK);
    CENTRE_REQUIRE(snapshot.providers == 1U && snapshot.profiles == 1U && snapshot.manifests == 1U && snapshot.units == 1U);
    CENTRE_REQUIRE(strcmp(snapshot.active_profile_id,"debug") == 0);
    for (unsigned corrupt = 0U; corrupt < 12U; ++corrupt) {
        CENTRE_REQUIRE(CentreFixture(centre) == 0);
        switch (corrupt) {
        case 0: centre->profile_count = UMI_COMPILER_MAX_PROFILES+1U; break;
        case 1: centre->manifest_count = SIZE_MAX; break;
        case 2: centre->providers.count = UMI_COMPILER_MAX_PROVIDERS+1U; break;
        case 3: centre->artifacts.count = SIZE_MAX; break;
        case 4: centre->cache.count = UMI_COMPILER_MAX_CACHE_ENTRIES+1U; break;
        case 5: centre->compilation_database.count = SIZE_MAX; break;
        case 6: (void)memset(centre->active_profile_id,'x',sizeof(centre->active_profile_id)); break;
        case 7: (void)memset(centre->profiles[0].display_name,'x',sizeof(centre->profiles[0].display_name)); break;
        case 8: (void)memset(centre->manifests[0].target,'x',sizeof(centre->manifests[0].target)); break;
        case 9: centre->compilation_database.count = 1U; break; /* Empty active record. */
        case 10: centre->profiles[0].profile_id[0] = '\0'; break;
        case 11: centre->manifests[0].project_id[0] = '\0'; break;
        }
        (void)memcpy(before,centre,sizeof(*centre));
        (void)memset(&plan,0xa5,sizeof(plan)); (void)memcpy(&plan_before,&plan,sizeof(plan));
        (void)memset(&snapshot,0x5a,sizeof(snapshot)); (void)memcpy(&snapshot_before,&snapshot,sizeof(snapshot));
        CENTRE_REQUIRE(!umi_compiler_centre_storage_valid(centre));
        CENTRE_REQUIRE(umi_compiler_centre_active_profile(centre) == NULL);
        CENTRE_REQUIRE(umi_compiler_centre_activate_profile(centre,"debug") == UMI_STATUS_INVALID_STATE);
        CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&centre->profiles[0]) == UMI_STATUS_INVALID_STATE);
        CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&centre->manifests[0]) == UMI_STATUS_INVALID_STATE);
        CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&plan) == UMI_STATUS_INVALID_STATE);
        CENTRE_REQUIRE(umi_compiler_centre_snapshot_checked(centre,&snapshot) == UMI_STATUS_INVALID_STATE);
        CENTRE_REQUIRE(memcmp(&snapshot,&snapshot_before,sizeof(snapshot)) == 0);
        CENTRE_REQUIRE(memcmp(&plan,&plan_before,sizeof(plan)) == 0);
        CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);
        umi_compiler_centre_snapshot(centre,&snapshot);
        UmiCompilerCentreSnapshot empty = {0};
        /* Compare fields, since structure padding is not a public value. */
        CENTRE_REQUIRE(snapshot.providers == empty.providers && snapshot.revision == 0U && snapshot.active_profile_id[0] == '\0');
    }
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    centre->revision = UINT64_MAX; (void)memcpy(before,centre,sizeof(*centre));
    CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&centre->profiles[0]) == UMI_STATUS_ALREADY_EXISTS);
    CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&centre->manifests[0]) == UMI_STATUS_ALREADY_EXISTS);
    CENTRE_REQUIRE(umi_compiler_centre_activate_profile(centre,"debug") == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(umi_compiler_centre_active_profile(centre) == &centre->profiles[0]);
    CENTRE_REQUIRE(umi_compiler_centre_snapshot_checked(centre,&snapshot) == UMI_STATUS_OK && snapshot.revision == UINT64_MAX);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    /* Bounded but malformed later metadata still blocks an earlier profile. */
    centre->profiles[1] = centre->profiles[0]; (void)strcpy(centre->profiles[1].profile_id,"late"); centre->profile_count = 2U;
    (void)memset(centre->profiles[1].provider_id,'x',sizeof(centre->profiles[1].provider_id));
    CENTRE_REQUIRE(umi_compiler_centre_active_profile(centre) == NULL);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    (void)memcpy(before,centre,sizeof(*centre));
    CENTRE_REQUIRE(umi_compiler_centre_snapshot_checked(centre,(UmiCompilerCentreSnapshot *)(void *)centre) == UMI_STATUS_INVALID_ARGUMENT);
    umi_compiler_centre_snapshot(centre,(UmiCompilerCentreSnapshot *)(void *)centre);
    CENTRE_REQUIRE(umi_compiler_centre_plan_unit(centre,"project","main",&centre->compilation_database.items[0].plan) == UMI_STATUS_INVALID_ARGUMENT);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);

    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    centre->profiles[1] = centre->profiles[0]; (void)strcpy(centre->profiles[1].profile_id,"second");
    CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&centre->profiles[1]) == UMI_STATUS_OK);
    CENTRE_REQUIRE(umi_compiler_centre_activate_profile(centre,centre->profiles[1].profile_id) == UMI_STATUS_OK);
    CENTRE_REQUIRE(strcmp(centre->active_profile_id,"second") == 0);
    CENTRE_REQUIRE(umi_compiler_centre_activate_profile(centre,centre->active_profile_id) == UMI_STATUS_OK);
    for (size_t index = 2U; index < UMI_COMPILER_MAX_PROFILES; ++index) {
        centre->profiles[index] = centre->profiles[0];
        (void)snprintf(centre->profiles[index].profile_id,sizeof(centre->profiles[index].profile_id),"profile-%zu",index);
        CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&centre->profiles[index]) == UMI_STATUS_OK);
    }
    (void)memcpy(before,centre,sizeof(*centre));
    CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&centre->profiles[0]) == UMI_STATUS_ALREADY_EXISTS);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);
    static UmiCompilerProfile extra_profile; extra_profile = centre->profiles[0]; (void)strcpy(extra_profile.profile_id,"extra");
    CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&extra_profile) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t index = 1U; index < UMI_COMPILER_MAX_MANIFESTS; ++index) {
        (void)memcpy(&centre->manifests[index],&centre->manifests[0],sizeof(centre->manifests[index]));
        (void)snprintf(centre->manifests[index].project_id,sizeof(centre->manifests[index].project_id),"project-%zu",index);
        CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&centre->manifests[index]) == UMI_STATUS_OK);
    }
    static UmiCompilerProjectManifest extra_manifest;
    (void)memcpy(&extra_manifest,&centre->manifests[0],sizeof(extra_manifest)); (void)strcpy(extra_manifest.project_id,"extra");
    (void)memcpy(before,centre,sizeof(*centre));
    CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&centre->manifests[0]) == UMI_STATUS_ALREADY_EXISTS);
    CENTRE_REQUIRE(umi_compiler_centre_add_manifest(centre,&extra_manifest) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(centre,before,sizeof(*centre)) == 0);
    free(before); free(centre);
    return 0;
}
