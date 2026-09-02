/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/session_snapshot.c
 *
 * PURPOSE:
 *   Capture and restore bounded session state while validating every panel against canonical metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/session_snapshot.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *target, size_t capacity, const char *source)
{
    int written;
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(target, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/*
 * Provide the application session snapshot capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_snapshot_capture(
    const UmiApplicationSession *session,
    UmiApplicationSessionSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_session_validate(session) != UMI_STATUS_OK || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    result = copy_text(out_snapshot->application_id, sizeof(out_snapshot->application_id),
                       session->experience->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    result = copy_text(out_snapshot->layout_id, sizeof(out_snapshot->layout_id),
                       session->layout->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->active_panel_count; ++index) {
        result = copy_text(out_snapshot->active_panel_ids[index],
                           sizeof(out_snapshot->active_panel_ids[index]),
                           session->active_panel_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    out_snapshot->active_panel_count = session->active_panel_count;
    out_snapshot->layout_locked = session->layout_locked;
    out_snapshot->revision = session->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the application session snapshot restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_snapshot_restore(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationSessionSnapshot *snapshot,
    UmiApplicationSession *out_session)
{
    size_t index;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || snapshot == NULL || out_session == NULL ||
        snapshot->structure_size != sizeof(*snapshot) ||
        strcmp(snapshot->application_id, experience->application_id) != 0 ||
        snapshot->active_panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_init(experience, out_session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    out_session->layout_locked = false;
    result = umi_application_session_select_layout(out_session, snapshot->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    out_session->active_panel_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot->active_panel_count; ++index) {
        result = umi_application_session_activate_panel(
            out_session, snapshot->active_panel_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    out_session->layout_locked = snapshot->layout_locked;
    out_session->revision = snapshot->revision;
    return UMI_STATUS_OK;
}
