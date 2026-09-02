/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_history_model.h
 *
 * PURPOSE:
 *   Model search history model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_HISTORY_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_HISTORY_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel search history model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSearchHistoryModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelSearchHistoryModel;
/**
 * Initialise editor intel search history model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_history_model_init(UmiEditorIntelSearchHistoryModel *model);
/**
 * Add editor intel search history model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_search_history_model_add(UmiEditorIntelSearchHistoryModel *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel search history model while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_search_history_model_find(const UmiEditorIntelSearchHistoryModel *model,const char *id);
/**
 * Release or reset state held by editor intel search history model so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_intel_search_history_model_clear(UmiEditorIntelSearchHistoryModel *model);
/**
 * Check that editor intel search history model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_search_history_model_valid(const UmiEditorIntelSearchHistoryModel *model);

#ifdef __cplusplus
}
#endif
#endif
