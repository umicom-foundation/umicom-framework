/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_profile_publication.c
 *
 * PURPOSE:
 *   Verify coherent provider/profile replacement, aliasing and complete failure preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "compiler_centre_fixture.h"
int main(void)
{
    UmiCompilerCentre *centre = malloc(sizeof(*centre)), *saved = malloc(sizeof(*saved));
    static UmiCompilerProfile profile;
    CENTRE_REQUIRE(centre != NULL && saved != NULL);
    CENTRE_REQUIRE(CentreFixture(centre) == 0);
    UmiCompilerProvider provider = umi_compiler_gcc_provider("replacement-driver","x86_64-unknown-linux-gnu");
    profile = centre->profiles[0];
    profile.policy.warnings_as_errors = true;
    uint64_t revision = centre->revision;
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,&provider,1U,&profile,true) == UMI_STATUS_OK);
    CENTRE_REQUIRE(centre->revision == revision+1U && centre->profile_count == 1U && centre->providers.count == 1U);
    CENTRE_REQUIRE(strcmp(centre->providers.items[0].descriptor.executable,"replacement-driver") == 0);
    CENTRE_REQUIRE(centre->profiles[0].policy.warnings_as_errors);
    CENTRE_REQUIRE(strcmp(centre->active_profile_id,"debug") == 0);
    /* A late profile refusal must roll back the proposed driver replacement too. */
    profile.policy.optimisation = (UmiCompilerOptimisation)-1;
    (void)strcpy(provider.descriptor.executable,"must-not-publish");
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,&provider,1U,&profile,true) != UMI_STATUS_OK);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    profile = centre->profiles[0];
    profile.enabled = false;
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,&provider,1U,&profile,true) == UMI_STATUS_INVALID_STATE);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    /* Existing identifiers remain replaceable when both collections are full. */
    for (size_t i = 1U; i < UMI_COMPILER_MAX_PROVIDERS; ++i) {
        provider = centre->providers.items[0];
        (void)snprintf(provider.descriptor.provider_id,sizeof(provider.descriptor.provider_id),"provider-%zu",i);
        CENTRE_REQUIRE(umi_compiler_registry_register(&centre->providers,&provider) == UMI_STATUS_OK);
    }
    for (size_t i = 1U; i < UMI_COMPILER_MAX_PROFILES; ++i) {
        profile = centre->profiles[0];
        (void)snprintf(profile.profile_id,sizeof(profile.profile_id),"profile-%zu",i);
        CENTRE_REQUIRE(umi_compiler_centre_add_profile(centre,&profile) == UMI_STATUS_OK);
    }
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,&centre->providers.items[0],1U,
                                                        &centre->profiles[0],true) == UMI_STATUS_OK);
    profile = centre->profiles[0];
    (void)strcpy(profile.profile_id,"new-profile");
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,NULL,0U,&profile,true) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    centre->providers.revision = UINT64_MAX;
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,&centre->providers.items[0],1U,
                                                        &centre->profiles[0],true) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    centre->revision = UINT64_MAX;
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,NULL,0U,&centre->profiles[0],true) == UMI_STATUS_CAPACITY_EXCEEDED);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    centre->profile_count = UMI_COMPILER_MAX_PROFILES+1U;
    (void)memcpy(saved,centre,sizeof(*saved));
    CENTRE_REQUIRE(umi_compiler_centre_configure_profile(centre,NULL,0U,&profile,false) == UMI_STATUS_INVALID_STATE);
    CENTRE_REQUIRE(memcmp(saved,centre,sizeof(*saved)) == 0);
    free(saved); free(centre);
    return 0;
}
