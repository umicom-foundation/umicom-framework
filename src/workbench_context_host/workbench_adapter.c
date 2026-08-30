/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/workbench_adapter.c
 *
 * PURPOSE:
 *   Convert the authoritative UI workbench snapshot into generic context-host observation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/workbench_adapter.h"
#include <string.h>
UmiStatus umi_workbench_context_host_observation_from_workbench(
    const UmiUiWorkbench *workbench,uint64_t now_ms,
    UmiWorkbenchContextHostObservation *out_observation)
{
    UmiUiWorkbenchSnapshot snapshot;UmiStatus s;
    if(!workbench||!out_observation)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_ui_workbench_snapshot(workbench,&snapshot);if(s!=UMI_STATUS_OK)return s;
    umi_workbench_context_host_observation_init(out_observation);
    (void)umi_workbench_context_host_copy_text(out_observation->workbench_id,
        sizeof(out_observation->workbench_id),snapshot.workbench_id);
    (void)umi_workbench_context_host_copy_text(out_observation->workspace_profile,
        sizeof(out_observation->workspace_profile),snapshot.active_workspace_profile);
    (void)umi_workbench_context_host_copy_text(out_observation->perspective_id,
        sizeof(out_observation->perspective_id),snapshot.active_perspective);
    (void)umi_workbench_context_host_copy_text(out_observation->document_id,
        sizeof(out_observation->document_id),snapshot.active_document_view);
    (void)umi_workbench_context_host_copy_text(out_observation->activity_id,
        sizeof(out_observation->activity_id),snapshot.active_activity);
    (void)umi_workbench_context_host_copy_text(out_observation->view_container_id,
        sizeof(out_observation->view_container_id),snapshot.active_view_container);
    out_observation->source_revision=snapshot.revision;out_observation->observed_at_ms=now_ms;
    return UMI_STATUS_OK;
}
