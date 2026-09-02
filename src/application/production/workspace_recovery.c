/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/workspace_recovery.c
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
#include "umicom/application/production/workspace_recovery.h"

#include <string.h>

/*
 * Provide the application production workspace recover operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_production_workspace_recover(
    const UmiApplicationProductionCheckpointStore *store,
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime,
    UmiApplicationProductionRecoveryReport *out_report)
{
    const UmiApplicationProductionWorkspaceCheckpoint *checkpoint;
    UmiApplicationSession restored;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || experience == NULL || out_runtime == NULL ||
        out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    status = umi_application_workspace_runtime_init(experience, out_runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_report->status = status;
        return status;
    }
    checkpoint = umi_application_production_checkpoint_store_latest(
        store, experience->application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (checkpoint == NULL) {
        out_report->status = UMI_STATUS_OK;
        return UMI_STATUS_OK;
    }
    out_report->checkpoint_found = 1;
    out_report->checkpoint_sequence = checkpoint->sequence;
    out_report->clean_shutdown = checkpoint->clean_shutdown;
    status = umi_application_session_snapshot_restore(
        experience, &checkpoint->session, &restored);
    out_report->status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_runtime->session = restored;
    out_report->restored = 1;
    out_report->restored_panel_count = restored.active_panel_count;
    return UMI_STATUS_OK;
}

