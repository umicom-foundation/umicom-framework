/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/language_support_matrix.c
 *
 * PURPOSE:
 *   Implement language support coverage by composing existing profile, binding and installed-tool readiness services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/language_support_matrix.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) {
        source = "";
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        length = capacity - 1U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
}

/*
 * Provide the developer language support matrix build operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_language_support_matrix_build(
    const UmiLanguageProfileRegistry *profiles,
    const UmiDeveloperToolchainBindingRegistry *bindings,
    const char *platform,
    const char *architecture,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data,
    UmiDeveloperLanguageSupportMatrix *out_matrix)
{
    size_t index;
    size_t profile_count;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profiles == NULL ||
        bindings == NULL ||
        out_matrix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_matrix, 0, sizeof(*out_matrix));
    profile_count = umi_language_profile_registry_count(profiles);

    /* Apply this branch only when its contract condition is satisfied. */
    if (profile_count > UMI_DEVELOPER_LANGUAGE_SUPPORT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile_count; ++index) {
        UmiLanguageProfileSnapshot profile;
        UmiDeveloperToolchainBindingSnapshot binding;
        UmiDeveloperToolchainReadiness readiness;
        UmiDeveloperLanguageSupportSnapshot *row;
        UmiStatus status;

        status = umi_language_profile_registry_at(
            profiles,
            index,
            &profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        row = &out_matrix->items[out_matrix->count++];
        copy_text(
            row->language_id,
            sizeof(row->language_id),
            profile.id);
        row->profile_capabilities = profile.capabilities;

        status = umi_developer_toolchain_binding_registry_resolve(
            bindings,
            profile.id,
            platform,
            architecture,
            0U,
            &binding);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            out_matrix->blocked_count += 1U;
            continue;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        row->binding_found = 1;
        out_matrix->binding_found_count += 1U;
        row->binding_capabilities = binding.capabilities;
        copy_text(
            row->binding_id,
            sizeof(row->binding_id),
            binding.id);

        status = umi_developer_toolchain_binding_readiness(
            &binding,
            probe,
            user_data,
            &readiness);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        row->supported_operation_count =
            readiness.supported_operation_count;
        row->ready_operation_count =
            readiness.ready_operation_count;
        row->missing_operation_count =
            readiness.missing_operation_count;

        /* Apply this operation only while the related capability or state is available. */
        if (row->supported_operation_count > 0U) {
            row->readiness_percent =
                (unsigned)(
                    (row->ready_operation_count * 100U) /
                    row->supported_operation_count);
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (row->missing_operation_count == 0U &&
            row->supported_operation_count > 0U) {
            out_matrix->fully_ready_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_matrix->blocked_count += 1U;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Find developer language support matrix while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperLanguageSupportSnapshot *
umi_developer_language_support_matrix_find(
    const UmiDeveloperLanguageSupportMatrix *matrix,
    const char *language_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || language_id == NULL) {
        return NULL;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < matrix->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                matrix->items[index].language_id,
                language_id) == 0) {
            return &matrix->items[index];
        }
    }

    return NULL;
}
