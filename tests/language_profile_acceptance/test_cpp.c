/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_cpp.c
 *
 * PURPOSE:
 *   Verify the existing cpp language profile remains registered with its established core capability.
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

int test_language_profile_acceptance_cpp(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileSnapshot profile;

    assert(umi_test_language_profile_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_find(
        registry,
        "cpp",
        &profile) == UMI_STATUS_OK);

    assert(profile.enabled);
    assert(strcmp(profile.id, "cpp") == 0);
    assert(umi_language_profile_supports(
        &profile,
        UMI_LANGUAGE_CAPABILITY_BUILD));

    umi_language_profile_registry_destroy(registry);
    return 0;
}
