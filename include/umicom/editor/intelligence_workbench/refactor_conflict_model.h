/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_conflict_model.h
 *
 * PURPOSE:
 *   Represent a conflict detected while validating a refactoring candidate.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CONFLICT_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CONFLICT_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor conflict model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelRefactorConflictModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorConflictModel;
/**
 * Initialise editor intel refactor conflict model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_conflict_model_init(UmiEditorIntelRefactorConflictModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel refactor conflict model set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_conflict_model_set_score(UmiEditorIntelRefactorConflictModel *model,uint32_t score);
/**
 * Find editor intel refactor conflict model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_refactor_conflict_model_set_selected(UmiEditorIntelRefactorConflictModel *model,bool selected);
/**
 * Check that editor intel refactor conflict model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_conflict_model_valid(const UmiEditorIntelRefactorConflictModel *model);

#ifdef __cplusplus
}
#endif
#endif
