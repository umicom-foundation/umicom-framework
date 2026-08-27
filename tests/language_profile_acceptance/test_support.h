/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_support.h
 *
 * PURPOSE:
 *   Provide small test-only helpers for creating the existing built-in language profile registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LANGUAGE_PROFILE_ACCEPTANCE_SUPPORT_H
#define UMICOM_TEST_LANGUAGE_PROFILE_ACCEPTANCE_SUPPORT_H

#include "umicom/language/profile.h"

static UmiStatus umi_test_language_profile_registry_create(
    UmiLanguageProfileRegistry **out_registry)
{
    UmiStatus status;

    status = umi_language_profile_registry_create(out_registry);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_language_profile_register_builtins(*out_registry);
    if (status != UMI_STATUS_OK) {
        umi_language_profile_registry_destroy(*out_registry);
        *out_registry = NULL;
    }

    return status;
}

#endif
