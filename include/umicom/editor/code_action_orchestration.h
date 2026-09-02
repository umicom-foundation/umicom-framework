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

/**
 * Represent the editor code action orchestration snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor code action orchestration data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionOrchestration
    UmiEditorCodeActionOrchestration;

/**
 * Initialise editor code action orchestration from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_code_action_orchestration_create(
    UmiEditorCodeActionOrchestration **out_orchestration);
/**
 * Release or reset state held by editor code action orchestration so the same storage can
 * be reused safely.
 */
void umi_editor_code_action_orchestration_destroy(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration providers operation used by this module and
 * its client applications.
 */
UmiEditorCodeActionProviderRegistry *
umi_editor_code_action_orchestration_providers(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration query operation used by this module and its
 * client applications.
 */
UmiEditorCodeActionQuery *umi_editor_code_action_orchestration_query(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration plan operation used by this module and its
 * client applications.
 */
UmiEditorRefactoringPlan *umi_editor_code_action_orchestration_plan(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration preview operation used by this module and
 * its client applications.
 */
UmiEditorRefactoringPreview *umi_editor_code_action_orchestration_preview(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration transaction operation used by this module
 * and its client applications.
 */
UmiEditorEditTransaction *umi_editor_code_action_orchestration_transaction(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration refresh preview operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_refresh_preview(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration prepare transaction operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_prepare_transaction(
    UmiEditorCodeActionOrchestration *orchestration,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
/**
 * Provide the editor code action orchestration commit transaction operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_commit_transaction(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_cancel(
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor code action orchestration snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_snapshot(
    const UmiEditorCodeActionOrchestration *orchestration,
    UmiEditorCodeActionOrchestrationSnapshot *out_snapshot);
/**
 * Provide the editor code action orchestration revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_code_action_orchestration_revision(
    const UmiEditorCodeActionOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_ACTION_ORCHESTRATION_H */
