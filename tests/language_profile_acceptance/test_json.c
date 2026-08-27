/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_json.c
 *
 * PURPOSE:
 *   Verify the existing json language profile remains registered with its established core capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_support.h"

int test_language_profile_acceptance_json(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileSnapshot profile;

    assert(umi_test_language_profile_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_find(
        registry,
        "json",
        &profile) == UMI_STATUS_OK);

    assert(profile.enabled);
    assert(strcmp(profile.id, "json") == 0);
    assert(umi_language_profile_supports(
        &profile,
        UMI_LANGUAGE_CAPABILITY_PREVIEW));

    umi_language_profile_registry_destroy(registry);
    return 0;
}
