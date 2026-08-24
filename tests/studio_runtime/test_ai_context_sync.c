/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_ai_context_sync.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime ai context sync.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/ai_context_sync.h"
int main(void)
{
    UmiStudioRuntimeSelectionRouter selection;
    UmiAiDeveloperCommandContext context;
    umi_studio_selection_router_init(&selection);
    assert(umi_studio_selection_router_ai_approval(
        &selection,"approval.1","Build")==UMI_STATUS_OK);
    assert(umi_studio_ai_context_sync(
        &selection,"tester",&context)==UMI_STATUS_OK);
    assert(strcmp(context.approval_id,"approval.1")==0);
    assert(strcmp(context.approved_by,"tester")==0);
    return 0;
}

