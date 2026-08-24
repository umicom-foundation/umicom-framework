/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_status_model.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime status model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/status_model.h"
int main(void)
{
    UmiStudioRuntimeStatusModel model;
    UmiIdeIntegrationPlatformSnapshot snapshot = {0};
    UmiStudioRuntimeSelectionRouter selection;
    umi_studio_status_model_init(&model);
    umi_studio_selection_router_init(&selection);
    (void)strcpy(snapshot.context.workspace_root,"workspace");
    snapshot.context.has_problems=1;
    snapshot.context.problems.errors=2U;
    snapshot.context.problems.warnings=1U;
    snapshot.workflow.ready=0;
    snapshot.workflow.blocked_count=2U;
    assert(umi_studio_status_model_build(
        &model,&snapshot,&selection)==UMI_STATUS_OK);
    assert(model.item_count == 10U);
    assert(umi_studio_status_model_find(
        &model,UMI_STUDIO_STATUS_PROBLEMS)->badge_count==2U);
    return 0;
}

