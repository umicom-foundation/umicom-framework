/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_zig.c
 *
 * PURPOSE:
 *   Verify the existing zig language profile remains registered with its established core capability.
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

/*
 * Exercise test language profile acceptance zig and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_zig(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileSnapshot profile;

    assert(umi_test_language_profile_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_find(
        registry,
        "zig",
        &profile) == UMI_STATUS_OK);

    assert(profile.enabled);
    assert(strcmp(profile.id, "zig") == 0);
    assert(umi_language_profile_supports(
        &profile,
        UMI_LANGUAGE_CAPABILITY_BUILD));

    umi_language_profile_registry_destroy(registry);
    return 0;
}
