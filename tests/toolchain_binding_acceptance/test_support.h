/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_support.h
 *
 * PURPOSE:
 *   Provide test-only helpers for the existing built-in developer toolchain binding registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_TOOLCHAIN_BINDING_ACCEPTANCE_SUPPORT_H
#define UMICOM_TEST_TOOLCHAIN_BINDING_ACCEPTANCE_SUPPORT_H

#include "umicom/developer/toolchain_readiness.h"

/**
 * Exercise test all programs available and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int umi_test_all_programs_available(
    const char *program,
    void *user_data)
{
    (void)user_data;
    return program != NULL && program[0] != '\0';
}

/**
 * Initialise test toolchain binding registry from caller-provided values so later
 * operations receive a known state.
 */
static UmiStatus umi_test_toolchain_binding_registry_create(
    UmiDeveloperToolchainBindingRegistry **out_registry)
{
    UmiStatus status;

    status = umi_developer_toolchain_binding_registry_create(
        out_registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_developer_toolchain_binding_register_builtins(
        *out_registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_developer_toolchain_binding_registry_destroy(
            *out_registry);
        *out_registry = NULL;
    }

    return status;
}

#endif
