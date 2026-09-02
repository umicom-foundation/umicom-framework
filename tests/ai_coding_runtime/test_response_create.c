/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_response_create.c
 *
 * PURPOSE:
 *   Verify workspace-backed AI coding runtime response create behavior.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    plan.revision = 1U;
    plan.plan_hash = 9U;
    (void)strcpy(plan.request.request_id, "request");

    umi_ai_response_init(&response);
    assert(umi_ai_response_set_text(
        &response,
        "UMICOM-CODING-RESPONSE/1\n"
        "SUMMARY-BEGIN\nCreated source.\nSUMMARY-END\n"
        "PATCH-BEGIN\nTITLE|Create source\n"
        "RATIONALE-BEGIN\nRequired source.\nRATIONALE-END\n"
        "FILE-BEGIN|CREATE|src/new.c\n"
        "CONTENT-BEGIN\nint value = 1;\nCONTENT-END\nFILE-END\n"
        "PATCH-END\n") == UMI_STATUS_OK);

    assert(umi_ai_coding_parse_response(
        &response, &plan, &adapter, &parsed) == UMI_STATUS_OK);
    assert(parsed.structured == 1);
    assert(parsed.has_patch == 1);
    assert(parsed.patch.file_count == 1U);
    assert(parsed.patch.files[0].operation == UMI_AI_CODING_PATCH_CREATE);

    return 0;
}
