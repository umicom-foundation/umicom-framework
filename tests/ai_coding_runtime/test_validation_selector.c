/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_validation_selector.c
 *
 * PURPOSE:
 *   Verify workspace-backed AI coding runtime validation selector behavior.
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
#include "umicom/ai_coding_runtime/validation_selector.h"

int main(void)
{

    TestWorkspace workspace;
    UmiAiCodingWorkspaceAdapter adapter;
    UmiAiCodingValidationSelection selection;
    UmiAiCodingValidationPlan plan;

    test_workspace_init(&workspace);
    assert(test_workspace_add(
        &workspace, "CMakeLists.txt", "cmake_minimum_required(VERSION 3.24)\n") ==
        UMI_STATUS_OK);
    assert(test_workspace_adapter(&workspace, &adapter) == UMI_STATUS_OK);

    assert(umi_ai_coding_validation_select(
        &adapter, "C:/work", &selection, &plan) == UMI_STATUS_OK);
    assert(selection.detected == 1);
    assert(strcmp(selection.profile_id, "cmake") == 0);
    assert(plan.step_count == 3U);

    return 0;
}
