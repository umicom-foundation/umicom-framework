/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_context_materializer.c
 *
 * PURPOSE:
 *   Verify workspace-backed AI coding runtime context materializer behavior.
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
#include "umicom/ai_coding_runtime/context_materializer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    TestWorkspace workspace;
    UmiAiCodingWorkspaceAdapter adapter;
    UmiAiCodingTaskPlan plan;
    UmiAiCodingMaterializedContext context;

    test_workspace_init(&workspace);
    assert(test_workspace_add(
        &workspace, "src/main.c", "int value = 1;\n") == UMI_STATUS_OK);
    assert(test_workspace_adapter(&workspace, &adapter) == UMI_STATUS_OK);

    (void)memset(&plan, 0, sizeof(plan));
    plan.ready = 1;
    plan.revision = 7U;
    plan.repository_context.file_count = 1U;
    (void)strcpy(
        plan.repository_context.files[0].path, "src/main.c");
    (void)strcpy(
        plan.repository_context.files[0].language_id, "c");
    plan.repository_context.files[0].estimated_tokens = 4U;
    (void)strcpy(plan.request.active_path, "src/main.c");

    assert(umi_ai_coding_materialize_context(
        &plan, &adapter, &context) == UMI_STATUS_OK);
    assert(context.file_count == 1U);
    assert(context.files[0].active == 1);
    assert(strstr(context.files[0].text, "value") != NULL);

    return 0;
}
