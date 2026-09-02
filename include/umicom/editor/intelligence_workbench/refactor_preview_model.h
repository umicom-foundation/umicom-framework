/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_preview_model.h
 *
 * PURPOSE:
 *   Collect previewable refactoring changes while preserving source locations.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PREVIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_PREVIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor preview model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelRefactorPreviewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRefactorPreviewModel;
/**
 * Initialise editor intel refactor preview model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_preview_model_init(UmiEditorIntelRefactorPreviewModel *model);
/**
 * Add editor intel refactor preview model only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_refactor_preview_model_add(UmiEditorIntelRefactorPreviewModel *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel refactor preview model while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_refactor_preview_model_find(const UmiEditorIntelRefactorPreviewModel *model,const char *id);
/**
 * Release or reset state held by editor intel refactor preview model so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_refactor_preview_model_clear(UmiEditorIntelRefactorPreviewModel *model);
/**
 * Check that editor intel refactor preview model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_preview_model_valid(const UmiEditorIntelRefactorPreviewModel *model);

#ifdef __cplusplus
}
#endif
#endif
