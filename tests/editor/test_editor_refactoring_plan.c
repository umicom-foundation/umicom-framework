/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_refactoring_plan.c
 *
 * PURPOSE:
 *   Implement the test editor refactoring plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_refactoring_plan.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/refactoring_plan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorRefactoringPlan *plan = NULL;
    UmiEditorRefactoringDescriptor descriptor = {0};
    UmiEditorWorkspaceTextEdit edit = {0};
    UmiEditorRefactoringPlanSnapshot snapshot;

    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_REFACTORING_PLAN_API_VERSION;
    (void)strcpy(descriptor.id, "rename-total");
    (void)strcpy(descriptor.action_id, "action-rename-total");
    (void)strcpy(descriptor.provider_id, "clangd");
    (void)strcpy(descriptor.title, "Rename total to grand_total");
    (void)strcpy(descriptor.document_uri, "file:///workspace/main.c");
    (void)strcpy(descriptor.symbol_id, "symbol-total");
    descriptor.kind = UMI_EDITOR_REFACTORING_RENAME;
    descriptor.risk = UMI_EDITOR_REFACTORING_RISK_SAFE;
    descriptor.supports_preview = 1;
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(edit.id, "rename-use-1");
    (void)strcpy(edit.provider_id, "clangd");
    assert(umi_editor_source_location_initialize(
               &edit.location, descriptor.document_uri, 1U, 4U) ==
           UMI_STATUS_OK);
    edit.location.byte_offset = 4U;
    edit.location.end_byte_offset = 9U;
    edit.location.end_column = 9U;
    (void)strcpy(edit.expected_text, "total");
    (void)strcpy(edit.replacement_text, "grand_total");
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    edit.required = 1;
    assert(umi_editor_refactoring_plan_create(&plan) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_begin(plan, &descriptor) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(plan, &edit) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_finalize(plan) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_snapshot(plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_REFACTORING_PLAN_READY);
    assert(snapshot.edit_count == 1U);
    assert(snapshot.required_edit_count == 1U);
    assert(umi_editor_refactoring_plan_mark_applied(plan) == UMI_STATUS_OK);
    umi_editor_refactoring_plan_destroy(plan);
    return 0;
}
