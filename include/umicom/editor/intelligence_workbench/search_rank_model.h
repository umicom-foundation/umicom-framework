/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_rank_model.h
 *
 * PURPOSE:
 *   Model search rank model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RANK_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RANK_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchRankModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchRankModel;
UmiStatus umi_editor_intel_search_rank_model_init(UmiEditorIntelSearchRankModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_search_rank_model_set_score(UmiEditorIntelSearchRankModel *model,uint32_t score);
UmiStatus umi_editor_intel_search_rank_model_set_selected(UmiEditorIntelSearchRankModel *model,bool selected);
int umi_editor_intel_search_rank_model_valid(const UmiEditorIntelSearchRankModel *model);

#ifdef __cplusplus
}
#endif
#endif
