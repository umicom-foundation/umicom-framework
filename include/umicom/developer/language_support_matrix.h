/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/language_support_matrix.h
 *
 * PURPOSE:
 *   Summarise existing language profiles, resolved toolchain bindings and installed-tool readiness for Studio-facing capability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_LANGUAGE_SUPPORT_MATRIX_H
#define UMICOM_DEVELOPER_LANGUAGE_SUPPORT_MATRIX_H

#include "umicom/developer/toolchain_readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_LANGUAGE_SUPPORT_CAPACITY \
    UMI_LANGUAGE_PROFILE_CAPACITY

/**
 * Represent the developer language support snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperLanguageSupportSnapshot {
    char language_id[UMI_LANGUAGE_PROFILE_ID_CAPACITY];
    char binding_id[128];
    UmiLanguageCapabilityFlags profile_capabilities;
    UmiLanguageCapabilityFlags binding_capabilities;
    size_t supported_operation_count;
    size_t ready_operation_count;
    size_t missing_operation_count;
    unsigned readiness_percent;
    int binding_found;
} UmiDeveloperLanguageSupportSnapshot;

/**
 * Represent the developer language support matrix data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperLanguageSupportMatrix {
    UmiDeveloperLanguageSupportSnapshot
        items[UMI_DEVELOPER_LANGUAGE_SUPPORT_CAPACITY];
    size_t count;
    size_t binding_found_count;
    size_t fully_ready_count;
    size_t blocked_count;
} UmiDeveloperLanguageSupportMatrix;

/*
 * Build one support matrix from the existing profile and binding registries.
 * Resolution uses the current platform/architecture selectors and does not
 * create a parallel language or toolchain catalogue.
 */
UmiStatus umi_developer_language_support_matrix_build(
    const UmiLanguageProfileRegistry *profiles,
    const UmiDeveloperToolchainBindingRegistry *bindings,
    const char *platform,
    const char *architecture,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data,
    UmiDeveloperLanguageSupportMatrix *out_matrix);

/* Find one matrix row by language id; returned storage remains matrix-owned. */
const UmiDeveloperLanguageSupportSnapshot *
umi_developer_language_support_matrix_find(
    const UmiDeveloperLanguageSupportMatrix *matrix,
    const char *language_id);

#ifdef __cplusplus
}
#endif
#endif
