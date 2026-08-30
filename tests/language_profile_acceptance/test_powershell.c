/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_powershell.c
 *
 * PURPOSE:
 *   Verify the existing powershell language profile remains registered with its established core capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_support.h"

int test_language_profile_acceptance_powershell(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileSnapshot profile;

    assert(umi_test_language_profile_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_find(
        registry,
        "powershell",
        &profile) == UMI_STATUS_OK);

    assert(profile.enabled);
    assert(strcmp(profile.id, "powershell") == 0);
    assert(umi_language_profile_supports(
        &profile,
        UMI_LANGUAGE_CAPABILITY_RUN));

    umi_language_profile_registry_destroy(registry);
    return 0;
}
