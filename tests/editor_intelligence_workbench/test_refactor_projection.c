/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_refactor_projection.c
 *
 * PURPOSE:
 *   Verify canonical preview selection, conflict state and bounded history.
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

#include "umicom/editor/intelligence_workbench/refactor_projection.h"

static UmiEditorWorkspaceTextEdit edit(
    const char *id, uint64_t start, int required)
{
    UmiEditorWorkspaceTextEdit item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///src/main.c", 0U, start) ==
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
    UmiEditorCodeActionOrchestration *orchestration = NULL;
    UmiEditorRefactoringDescriptor descriptor;
    UmiEditorWorkspaceTextEdit required_edit = edit("required", 0U, 1);
    UmiEditorWorkspaceTextEdit optional_edit = edit("optional", 2U, 0);
    UmiEditorIntelRefactorProjection projection;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_REFACTORING_PLAN_API_VERSION;
    (void)strcpy(descriptor.id, "rename-x");
    (void)strcpy(descriptor.action_id, "rename-action");
    (void)strcpy(descriptor.provider_id, "native-c");
    (void)strcpy(descriptor.title, "Rename x to y");
    (void)strcpy(descriptor.document_uri, "file:///src/main.c");
    descriptor.kind = UMI_EDITOR_REFACTORING_RENAME;
    descriptor.risk = UMI_EDITOR_REFACTORING_RISK_SAFE;
    descriptor.supports_preview = 1;
    assert(umi_editor_code_action_orchestration_create(&orchestration) ==
           UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_begin(
               umi_editor_code_action_orchestration_plan(orchestration),
               &descriptor) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(
               umi_editor_code_action_orchestration_plan(orchestration),
               &required_edit) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(
               umi_editor_code_action_orchestration_plan(orchestration),
               &optional_edit) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_finalize(
               umi_editor_code_action_orchestration_plan(orchestration)) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_orchestration_refresh_preview(orchestration) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_refactor_projection_init(&projection) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_refactor_projection_refresh(
               &projection, orchestration) == UMI_STATUS_OK);
    assert(projection.preview.count == 2U);
    assert(projection.phase == UMI_EDITOR_INTEL_PHASE_READY);
    assert(umi_editor_intel_refactor_projection_set_included(
               &projection, orchestration, 1U, 0) == UMI_STATUS_OK);
    assert((projection.preview.items[1].flags &
            UMI_EDITOR_INTEL_PROJECTION_SELECTED) == 0U);
    assert(umi_editor_intel_refactor_projection_record_history(
               &projection, "Prepared rename") == UMI_STATUS_OK);
    assert(projection.history.count == 1U);
    assert(umi_editor_intel_refactor_projection_valid(&projection));
    umi_editor_code_action_orchestration_destroy(orchestration);
    return 0;
}
