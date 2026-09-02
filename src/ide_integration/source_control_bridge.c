/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/source_control_bridge.c
 *
 * PURPOSE:
 *   Implement Source Control -> editor/AI projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/source_control_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the ide source control target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_source_control_target(
    const char *path,
    uint32_t line,
    UmiIdeNavigationTarget *out_target)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_SOURCE_CONTROL;
    out_target->reason = UMI_IDE_NAVIGATION_SOURCE_CONTROL;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "source-control.file");
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "%.240s",
        path);

    umi_ide_location_init(&out_target->location);

    status = umi_ide_location_set_path(
        &out_target->location,
        path,
        line,
        0U);
    return status;
}

/*
 * Provide the ide source control summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_source_control_summary(
    const UmiDeveloperSourceControl *source_control,
    char *out_text,
    size_t capacity)
{
    UmiDeveloperSourceControlSnapshot snapshot;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_control == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_source_control_snapshot(
        source_control,
        &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        out_text,
        capacity,
        "Source control provider=%.120s branch=%.180s changes=%zu staged=%zu "
        "available=%d lastStatus=%u message=%.1000s",
        snapshot.provider_id,
        snapshot.branch.name,
        snapshot.change_count,
        snapshot.staged_count,
        snapshot.available,
        (unsigned)snapshot.last_status,
        snapshot.last_message);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
