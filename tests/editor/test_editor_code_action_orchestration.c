/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_code_action_orchestration.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/code_action_orchestration.h"

int main(void)
{
    UmiEditorCodeActionOrchestration *orchestration = NULL;
    UmiEditorCodeActionOrchestrationSnapshot snapshot;
    UmiEditorRefactoringDescriptor descriptor = {0};
    UmiEditorWorkspaceTextEdit edit = {0};
    UmiEditorEditTransactionDocument document = {0};
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;

    assert(umi_editor_code_action_orchestration_create(&orchestration) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_orchestration_providers(orchestration) != NULL);
    assert(umi_editor_code_action_orchestration_query(orchestration) != NULL);
    assert(umi_editor_code_action_orchestration_plan(orchestration) != NULL);
    assert(umi_editor_code_action_orchestration_preview(orchestration) != NULL);
    assert(umi_editor_code_action_orchestration_transaction(orchestration) !=
           NULL);
    assert(umi_editor_code_action_orchestration_snapshot(orchestration,
                                                         &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.plan_state == UMI_EDITOR_REFACTORING_PLAN_EMPTY);
    assert(snapshot.transaction_state == UMI_EDITOR_EDIT_TRANSACTION_EMPTY);
    assert(snapshot.quick_fixes_enabled);
    assert(snapshot.refactorings_enabled);
    assert(snapshot.preview_enabled);
    assert(snapshot.atomic_transactions_enabled);

    assert(umi_editor_text_buffer_create(0U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "old", 3U) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_EDITOR_REFACTORING_PLAN_API_VERSION;
    (void)strcpy(descriptor.id, "rename-old");
    (void)strcpy(descriptor.action_id, "rename-action");
    (void)strcpy(descriptor.provider_id, "native-c");
    (void)strcpy(descriptor.title, "Rename old to new");
    (void)strcpy(descriptor.document_uri, "file:///main.c");
    descriptor.kind = UMI_EDITOR_REFACTORING_RENAME;
    descriptor.risk = UMI_EDITOR_REFACTORING_RISK_SAFE;
    descriptor.supports_preview = 1;
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(edit.id, "rename-use");
    (void)strcpy(edit.provider_id, "native-c");
    assert(umi_editor_source_location_initialize(
               &edit.location, descriptor.document_uri, 0U, 0U) ==
           UMI_STATUS_OK);
    edit.location.end_byte_offset = 3U;
    edit.location.end_column = 3U;
    edit.location.document_revision = view.revision;
    (void)strcpy(edit.expected_text, "old");
    (void)strcpy(edit.replacement_text, "new");
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    edit.required = 1;
    assert(umi_editor_refactoring_plan_begin(
               umi_editor_code_action_orchestration_plan(orchestration),
               &descriptor) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_add_edit(
               umi_editor_code_action_orchestration_plan(orchestration),
               &edit) == UMI_STATUS_OK);
    assert(umi_editor_refactoring_plan_finalize(
               umi_editor_code_action_orchestration_plan(orchestration)) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_orchestration_refresh_preview(orchestration) ==
           UMI_STATUS_OK);
    document.struct_size = (uint32_t)sizeof(document);
    document.api_version = UMI_EDITOR_EDIT_TRANSACTION_API_VERSION;
    (void)strcpy(document.uri, descriptor.document_uri);
    document.buffer = buffer;
    document.writable = 1;
    document.require_matching_revision = 1;
    assert(umi_editor_code_action_orchestration_prepare_transaction(
               orchestration, &document, 1U) == UMI_STATUS_OK);
    assert(umi_editor_code_action_orchestration_commit_transaction(
               orchestration) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "new") == 0);
    assert(umi_editor_code_action_orchestration_snapshot(orchestration,
                                                         &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.plan_state == UMI_EDITOR_REFACTORING_PLAN_APPLIED);
    assert(snapshot.transaction_state == UMI_EDITOR_EDIT_TRANSACTION_COMMITTED);
    umi_editor_text_buffer_destroy(buffer);
    umi_editor_code_action_orchestration_destroy(orchestration);
    return 0;
}
