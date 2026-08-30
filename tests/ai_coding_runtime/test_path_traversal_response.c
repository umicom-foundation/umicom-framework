/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_path_traversal_response.c
 *
 * PURPOSE:
 *   Verify workspace-backed AI coding runtime path traversal response behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "workspace_test_support.h"
#include "umicom/ai_coding_runtime/response.h"

int main(void)
{

    TestWorkspace workspace;
    UmiAiCodingWorkspaceAdapter adapter;
    UmiAiCodingTaskPlan plan;
    UmiAiResponse response;
    UmiAiCodingParsedResponse parsed;

    test_workspace_init(&workspace);
    assert(test_workspace_adapter(&workspace, &adapter) == UMI_STATUS_OK);
    (void)memset(&plan, 0, sizeof(plan));
    plan.ready = 1;
    plan.plan_hash = 11U;
    (void)strcpy(plan.request.request_id, "request");

    umi_ai_response_init(&response);
    assert(umi_ai_response_set_text(
        &response,
        "UMICOM-CODING-RESPONSE/1\n"
        "PATCH-BEGIN\nTITLE|Bad\nRATIONALE-BEGIN\nBad.\nRATIONALE-END\n"
        "FILE-BEGIN|CREATE|../secret.txt\n"
        "CONTENT-BEGIN\nsecret\nCONTENT-END\nFILE-END\nPATCH-END\n") ==
        UMI_STATUS_OK);

    assert(umi_ai_coding_parse_response(
        &response, &plan, &adapter, &parsed) ==
        UMI_STATUS_PERMISSION_DENIED);

    return 0;
}
