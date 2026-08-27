/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_query_model.h
 *
 * PURPOSE:
 *   Represent one immutable workspace search query and its revision.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_QUERY_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_QUERY_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchQueryModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchQueryModel;
UmiStatus umi_editor_intel_search_query_model_init(UmiEditorIntelSearchQueryModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_search_query_model_set_score(UmiEditorIntelSearchQueryModel *model,uint32_t score);
UmiStatus umi_editor_intel_search_query_model_set_selected(UmiEditorIntelSearchQueryModel *model,bool selected);
int umi_editor_intel_search_query_model_valid(const UmiEditorIntelSearchQueryModel *model);

#ifdef __cplusplus
}
#endif
#endif
