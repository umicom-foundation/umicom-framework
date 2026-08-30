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

typedef struct UmiEditorIntelSearchHistoryModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelSearchHistoryModel;
UmiStatus umi_editor_intel_search_history_model_init(UmiEditorIntelSearchHistoryModel *model);
UmiStatus umi_editor_intel_search_history_model_add(UmiEditorIntelSearchHistoryModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_search_history_model_find(const UmiEditorIntelSearchHistoryModel *model,const char *id);
UmiStatus umi_editor_intel_search_history_model_clear(UmiEditorIntelSearchHistoryModel *model);
int umi_editor_intel_search_history_model_valid(const UmiEditorIntelSearchHistoryModel *model);

#ifdef __cplusplus
}
#endif
#endif
