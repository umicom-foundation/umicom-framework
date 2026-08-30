/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action_orchestration.h
 *
 * PURPOSE:
 *   Coordinate Framework-owned provider discovery, code-action ranking,
 *   refactoring planning, preview and atomic edit transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_CODE_ACTION_ORCHESTRATION_H
#define UMICOM_EDITOR_CODE_ACTION_ORCHESTRATION_H

#include "umicom/editor/code_action_query.h"
#include "umicom/editor/edit_transaction.h"
#include "umicom/editor/refactoring_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_CODE_ACTION_ORCHESTRATION_API_VERSION 1U

typedef struct UmiEditorCodeActionOrchestrationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t action_count;
    size_t refactoring_edit_count;
    size_t preview_item_count;
    UmiEditorRefactoringPlanState plan_state;
    UmiEditorEditTransactionState transaction_state;
    uint64_t revision;
    int quick_fixes_enabled;
    int refactorings_enabled;
    int preview_enabled;
    int atomic_transactions_enabled;
} UmiEditorCodeActionOrchestrationSnapshot;

typedef struct UmiEditorCodeActionOrchestration
    UmiEditorCodeActionOrchestration;

UmiStatus umi_editor_code_action_orchestration_create(
    UmiEditorCodeActionOrchestration **out_orchestration);
void umi_editor_code_action_orchestration_destroy(
    UmiEditorCodeActionOrchestration *orchestration);
UmiEditorCodeActionProviderRegistry *
umi_editor_code_action_orchestration_providers(
    UmiEditorCodeActionOrchestration *orchestration);
UmiEditorCodeActionQuery *umi_editor_code_action_orchestration_query(
    UmiEditorCodeActionOrchestration *orchestration);
UmiEditorRefactoringPlan *umi_editor_code_action_orchestration_plan(
    UmiEditorCodeActionOrchestration *orchestration);
UmiEditorRefactoringPreview *umi_editor_code_action_orchestration_preview(
    UmiEditorCodeActionOrchestration *orchestration);
UmiEditorEditTransaction *umi_editor_code_action_orchestration_transaction(
    UmiEditorCodeActionOrchestration *orchestration);
UmiStatus umi_editor_code_action_orchestration_refresh_preview(
    UmiEditorCodeActionOrchestration *orchestration);
UmiStatus umi_editor_code_action_orchestration_prepare_transaction(
    UmiEditorCodeActionOrchestration *orchestration,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
UmiStatus umi_editor_code_action_orchestration_commit_transaction(
    UmiEditorCodeActionOrchestration *orchestration);
UmiStatus umi_editor_code_action_orchestration_cancel(
    UmiEditorCodeActionOrchestration *orchestration);
UmiStatus umi_editor_code_action_orchestration_snapshot(
    const UmiEditorCodeActionOrchestration *orchestration,
    UmiEditorCodeActionOrchestrationSnapshot *out_snapshot);
uint64_t umi_editor_code_action_orchestration_revision(
    const UmiEditorCodeActionOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_ACTION_ORCHESTRATION_H */
