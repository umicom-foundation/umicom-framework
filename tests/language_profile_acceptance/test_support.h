/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_support.h
 *
 * PURPOSE:
 *   Provide small test-only helpers for creating the existing built-in language profile registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LANGUAGE_PROFILE_ACCEPTANCE_SUPPORT_H
#define UMICOM_TEST_LANGUAGE_PROFILE_ACCEPTANCE_SUPPORT_H

#include "umicom/language/profile.h"

/**
 * Initialise test language profile registry from caller-provided values so later
 * operations receive a known state.
 */
static UmiStatus umi_test_language_profile_registry_create(
    UmiLanguageProfileRegistry **out_registry)
{
    UmiStatus status;

    status = umi_language_profile_registry_create(out_registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_language_profile_register_builtins(*out_registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_language_profile_registry_destroy(*out_registry);
        *out_registry = NULL;
    }

    return status;
}

#endif
