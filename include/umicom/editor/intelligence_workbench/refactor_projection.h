/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_projection.h
 *
 * PURPOSE:
 *   Project canonical refactoring plan, preview and transaction state into one
 *   revisioned workbench model with selectable edits and bounded history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PROJECTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PROJECTION_H

#include "umicom/editor/code_action_orchestration.h"
#include "umicom/editor/intelligence_workbench/refactor_history.h"
#include "umicom/editor/intelligence_workbench/refactor_preview_model.h"
#include "umicom/editor/intelligence_workbench/projection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_REFACTOR_PROJECTION_API_VERSION 1U

/**
 * Represent the editor intel refactor projection data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorProjection {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIntelRefactorPreviewModel preview;
    UmiEditorIntelRefactorHistory history;
    UmiEditorRefactoringPlanSnapshot plan_snapshot;
    UmiEditorRefactoringPreviewSnapshot preview_snapshot;
    UmiEditorEditTransactionSnapshot transaction_snapshot;
    UmiEditorIntelPhase phase;
    size_t selected_index;
    uint64_t source_revision;
    uint64_t revision;
    int has_selection;
    int has_conflict;
} UmiEditorIntelRefactorProjection;

/**
 * Initialise editor intel refactor projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_projection_init(
    UmiEditorIntelRefactorProjection *projection);
/**
 * Provide the editor intel refactor projection refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_refresh(
    UmiEditorIntelRefactorProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor intel refactor projection set included operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_set_included(
    UmiEditorIntelRefactorProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration,
    size_t index,
    int included);
/**
 * Provide the editor intel refactor projection record history operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_record_history(
    UmiEditorIntelRefactorProjection *projection,
    const char *label);
/**
 * Find editor intel refactor projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_refactor_projection_selected(
    const UmiEditorIntelRefactorProjection *projection);
/**
 * Check that editor intel refactor projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_projection_valid(
    const UmiEditorIntelRefactorProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PROJECTION_H */
