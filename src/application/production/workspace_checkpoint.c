/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/workspace_checkpoint.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/workspace_checkpoint.h"

#include <string.h>

UmiStatus umi_application_production_workspace_checkpoint_capture(
    const UmiApplicationWorkspaceRuntime *runtime, uint64_t sequence,
    const char *reason, int clean_shutdown,
    UmiApplicationProductionWorkspaceCheckpoint *out_checkpoint)
{
    size_t length;
    UmiStatus status;
    if (runtime == NULL || reason == NULL || out_checkpoint == NULL ||
        sequence == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(reason);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= sizeof(out_checkpoint->reason))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_checkpoint, 0, sizeof(*out_checkpoint));
    status = umi_application_session_snapshot_capture(
        &runtime->session, &out_checkpoint->session);
    if (status != UMI_STATUS_OK) return status;
    out_checkpoint->sequence = sequence;
    (void)memcpy(out_checkpoint->reason, reason, length + 1U);
    out_checkpoint->clean_shutdown = clean_shutdown != 0;
    return umi_application_production_workspace_checkpoint_validate(
        out_checkpoint);
}

UmiStatus umi_application_production_workspace_checkpoint_validate(
    const UmiApplicationProductionWorkspaceCheckpoint *checkpoint)
{
    if (checkpoint == NULL || checkpoint->sequence == 0U ||
        checkpoint->reason[0] == '\0' ||
        checkpoint->session.application_id[0] == '\0' ||
        checkpoint->session.layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

