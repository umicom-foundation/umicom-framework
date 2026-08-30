/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_workspace_adapter.c
 *
 * PURPOSE:
 *   Verify workspace-backed AI coding runtime workspace adapter behavior.
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
#include "umicom/ai_coding_runtime/workspace.h"

int main(void)
{

    TestWorkspace workspace;
    UmiAiCodingWorkspaceAdapter adapter;
    char text[128];
    size_t length = 0U;
    int exists = 0;

    test_workspace_init(&workspace);
    assert(test_workspace_add(
        &workspace, "src/main.c", "int main(void){}\n") == UMI_STATUS_OK);
    assert(test_workspace_adapter(&workspace, &adapter) == UMI_STATUS_OK);
    assert(umi_ai_coding_workspace_adapter_validate(&adapter) == UMI_STATUS_OK);
    assert(adapter.exists(
        adapter.user_data, "src/main.c", &exists) == UMI_STATUS_OK);
    assert(exists);
    assert(adapter.read(
        adapter.user_data, "src/main.c", text, sizeof(text), &length) ==
        UMI_STATUS_OK);
    assert(length == strlen(text));

    return 0;
}
