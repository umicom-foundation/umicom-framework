/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/reference_results_model.h
 *
 * PURPOSE:
 *   Model reference results model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFERENCE_RESULTS_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFERENCE_RESULTS_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel reference results model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelReferenceResultsModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelReferenceResultsModel;
/**
 * Initialise editor intel reference results model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_reference_results_model_init(UmiEditorIntelReferenceResultsModel *model);
/**
 * Add editor intel reference results model only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_reference_results_model_add(UmiEditorIntelReferenceResultsModel *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel reference results model while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_reference_results_model_find(const UmiEditorIntelReferenceResultsModel *model,const char *id);
/**
 * Release or reset state held by editor intel reference results model so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_reference_results_model_clear(UmiEditorIntelReferenceResultsModel *model);
/**
 * Check that editor intel reference results model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_reference_results_model_valid(const UmiEditorIntelReferenceResultsModel *model);

#ifdef __cplusplus
}
#endif
#endif
