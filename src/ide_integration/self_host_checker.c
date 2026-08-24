/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/self_host_checker.c
 *
 * PURPOSE:
 *   Implement self-hosting surface/command/capability closure verification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/self_host_checker.h"

#include <string.h>

static UmiStatus add_missing(
    char items[UMI_IDE_SELF_HOST_MISSING_CAPACITY]
              [UMI_IDE_INTEGRATION_ID_CAPACITY],
    size_t *count,
    const char *value)
{
    size_t length;

    if (*count >= UMI_IDE_SELF_HOST_MISSING_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    length = strlen(value);
    if (length >= UMI_IDE_INTEGRATION_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(items[*count], value, length + 1U);
    *count += 1U;
    return UMI_STATUS_OK;
}

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

    if (manifest == NULL || surfaces == NULL ||
        commands == NULL || capability_lookup == NULL ||
        out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_self_host_manifest_validate(manifest);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->revision = 1U;
    out_report->ready = 1;

    for (index = 0U; index < manifest->required_surface_count; ++index) {
        if (umi_ide_surface_registry_find(
                surfaces,
                manifest->required_surface_ids[index]) == NULL) {
            status = add_missing(
                out_report->missing_surfaces,
                &out_report->missing_surface_count,
                manifest->required_surface_ids[index]);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    for (index = 0U; index < manifest->required_command_count; ++index) {
        if (!umi_command_registry_contains(
                commands,
                manifest->required_command_ids[index])) {
            status = add_missing(
                out_report->missing_commands,
                &out_report->missing_command_count,
                manifest->required_command_ids[index]);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    for (index = 0U; index < manifest->required_capability_count; ++index) {
        if (!capability_lookup(
                capability_user_data,
                manifest->required_capabilities[index])) {
            status = add_missing(
                out_report->missing_capabilities,
                &out_report->missing_capability_count,
                manifest->required_capabilities[index]);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    out_report->ready =
        out_report->missing_surface_count == 0U &&
        out_report->missing_command_count == 0U &&
        out_report->missing_capability_count == 0U;

    return UMI_STATUS_OK;
}
