/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_filter_model.h
 *
 * PURPOSE:
 *   Model search filter model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_FILTER_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_FILTER_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchFilterModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchFilterModel;
UmiStatus umi_editor_intel_search_filter_model_init(UmiEditorIntelSearchFilterModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_search_filter_model_set_score(UmiEditorIntelSearchFilterModel *model,uint32_t score);
UmiStatus umi_editor_intel_search_filter_model_set_selected(UmiEditorIntelSearchFilterModel *model,bool selected);
int umi_editor_intel_search_filter_model_valid(const UmiEditorIntelSearchFilterModel *model);

#ifdef __cplusplus
}
#endif
#endif
