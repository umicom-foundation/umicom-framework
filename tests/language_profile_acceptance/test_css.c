/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_css.c
 *
 * PURPOSE:
 *   Verify the existing css language profile remains registered with its established core capability.
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
 * Exercise test language profile acceptance css and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_profile_acceptance_css(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileSnapshot profile;

    assert(umi_test_language_profile_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_find(
        registry,
        "css",
        &profile) == UMI_STATUS_OK);

    assert(profile.enabled);
    assert(strcmp(profile.id, "css") == 0);
    assert(umi_language_profile_supports(
        &profile,
        UMI_LANGUAGE_CAPABILITY_PREVIEW));

    umi_language_profile_registry_destroy(registry);
    return 0;
}
