/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_refactoring_preview.c
 *
 * PURPOSE:
 *   Implement the test editor refactoring preview behavior for
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
 * File: tests/editor/test_editor_refactoring_preview.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/refactoring_preview.h"

static UmiEditorWorkspaceTextEdit edit(const char *id, uint64_t start,
                                       int required)
{
    UmiEditorWorkspaceTextEdit item = {0};
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/main.c", 0U, start) ==
           UMI_STATUS_OK);
    item.location.byte_offset = start;
    item.location.end_byte_offset = start + 1U;
    item.location.end_column = start + 1U;
    (void)strcpy(item.expected_text, "x");
    (void)strcpy(item.replacement_text, "y");
    item.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    item.required = required;
    return item;
}

int main(void)
{
    UmiEditorRefactoringPlan *plan = NULL;
    UmiEditorRefactoringPreview *preview = NULL;
    UmiEditorWorkspaceEditSet *selected = NULL;
    UmiEditorRefactoringDescriptor descriptor = {0};
    UmiEditorWorkspaceTextEdit required_edit = edit("required", 0U, 1);
    UmiEditorWorkspaceTextEdit optional_edit = edit("optional", 2U, 0);
    UmiEditorRefactoringPreviewSnapshot snapshot;

    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_REFACTORING_PLAN_API_VERSION;
    (void)strcpy(descriptor.id, "rename-x");
    (void)strcpy(descriptor.action_id, "rename-action");
    (void)strcpy(descriptor.provider_id, "native-c");
    (void)strcpy(descriptor.title, "Rename x to y");
    (void)strcpy(descriptor.document_uri, "file:///workspace/main.c");
    descriptor.kind = UMI_EDITOR_REFACTORING_RENAME;
    descriptor.risk = UMI_EDITOR_REFACTORING_RISK_SAFE;
    descriptor.supports_preview = 1;
    assert(umi_editor_refactoring_plan_create(&plan) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_preview_create(&preview) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_create(&selected) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_begin(plan, &descriptor) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(plan, &required_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(plan, &optional_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_finalize(plan) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_preview_build(preview, plan) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_preview_set_included(preview, "optional", 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_preview_set_included(preview, "required", 0) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_editor_refactoring_preview_selected_edits(preview, selected) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_count(selected) == 1U);
    assert(umi_editor_refactoring_preview_snapshot(preview, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.included_count == 1U);
    assert(snapshot.excluded_count == 1U);
    umi_editor_workspace_edit_set_destroy(selected);
    umi_editor_refactoring_preview_destroy(preview);
    umi_editor_refactoring_plan_destroy(plan);
    return 0;
}
