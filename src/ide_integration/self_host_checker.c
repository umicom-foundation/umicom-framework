/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/self_host_checker.c
 *
 * PURPOSE:
 *   Implement self-hosting surface/command/capability closure verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/self_host_checker.h"

#include <string.h>

/* Provide the add missing operation used by this module and its client applications. */
static UmiStatus add_missing(
    char items[UMI_IDE_SELF_HOST_MISSING_CAPACITY]
              [UMI_IDE_INTEGRATION_ID_CAPACITY],
    size_t *count,
    const char *value)
{
    size_t length;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count >= UMI_IDE_SELF_HOST_MISSING_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_IDE_INTEGRATION_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(items[*count], value, length + 1U);
    *count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide self host check operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_self_host_check(
    const UmiIdeSelfHostManifest *manifest,
    const UmiIdeSurfaceRegistry *surfaces,
    const UmiCommandRegistry *commands,
    UmiIdeCapabilityLookup capability_lookup,
    void *capability_user_data,
    UmiIdeSelfHostCheckReport *out_report)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || surfaces == NULL ||
        commands == NULL || capability_lookup == NULL ||
        out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_self_host_manifest_validate(manifest);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->revision = 1U;
    out_report->ready = 1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->required_surface_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_ide_surface_registry_find(
                surfaces,
                manifest->required_surface_ids[index]) == NULL) {
            status = add_missing(
                out_report->missing_surfaces,
                &out_report->missing_surface_count,
                manifest->required_surface_ids[index]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->required_command_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_command_registry_contains(
                commands,
                manifest->required_command_ids[index])) {
            status = add_missing(
                out_report->missing_commands,
                &out_report->missing_command_count,
                manifest->required_command_ids[index]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->required_capability_count; ++index) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!capability_lookup(
                capability_user_data,
                manifest->required_capabilities[index])) {
            status = add_missing(
                out_report->missing_capabilities,
                &out_report->missing_capability_count,
                manifest->required_capabilities[index]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    out_report->ready =
        out_report->missing_surface_count == 0U &&
        out_report->missing_command_count == 0U &&
        out_report->missing_capability_count == 0U;

    return UMI_STATUS_OK;
}
