/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/workspace_checkpoint.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/workspace_checkpoint.h"

#include <string.h>

/*
 * Provide the application production workspace checkpoint capture operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_workspace_checkpoint_capture(
    const UmiApplicationWorkspaceRuntime *runtime, uint64_t sequence,
    const char *reason, int clean_shutdown,
    UmiApplicationProductionWorkspaceCheckpoint *out_checkpoint)
{
    size_t length;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || reason == NULL || out_checkpoint == NULL ||
        sequence == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(reason);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(out_checkpoint->reason))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_checkpoint, 0, sizeof(*out_checkpoint));
    status = umi_application_session_snapshot_capture(
        &runtime->session, &out_checkpoint->session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_checkpoint->sequence = sequence;
    (void)memcpy(out_checkpoint->reason, reason, length + 1U);
    out_checkpoint->clean_shutdown = clean_shutdown != 0;
    return umi_application_production_workspace_checkpoint_validate(
        out_checkpoint);
}

/*
 * Check that application production workspace checkpoint satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_production_workspace_checkpoint_validate(
    const UmiApplicationProductionWorkspaceCheckpoint *checkpoint)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (checkpoint == NULL || checkpoint->sequence == 0U ||
        checkpoint->reason[0] == '\0' ||
        checkpoint->session.application_id[0] == '\0' ||
        checkpoint->session.layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

