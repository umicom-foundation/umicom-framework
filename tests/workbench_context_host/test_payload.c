/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_payload.c
 *
 * PURPOSE:
 *   Verify observation payload enrichment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostObservation observation;
    UmiContextPayload payload;
    umi_workbench_context_host_observation_init(&observation);
    assert(umi_workbench_context_host_copy_text(
        observation.workspace_profile, sizeof(observation.workspace_profile),
        "develop") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        observation.document_id, sizeof(observation.document_id),
        "editor.main") == UMI_STATUS_OK);
    observation.source_revision = 42U;
    assert(umi_workbench_context_host_payload_from_observation(
        &payload, "context", "org.umicom.test", "observer", &observation) == UMI_STATUS_OK);
    assert(payload.kind == UMI_CONTEXT_KIND_SELECTION);
    assert(umi_context_payload_find_value(
        &payload, "workspace-profile") != NULL);
    assert(umi_context_payload_find_value(
        &payload, "workbench-revision") != NULL);
    return 0;
}
