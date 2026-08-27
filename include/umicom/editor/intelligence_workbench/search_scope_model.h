/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_scope_model.h
 *
 * PURPOSE:
 *   Model search scope model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_SCOPE_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_SCOPE_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchScopeModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchScopeModel;
UmiStatus umi_editor_intel_search_scope_model_init(UmiEditorIntelSearchScopeModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_search_scope_model_set_score(UmiEditorIntelSearchScopeModel *model,uint32_t score);
UmiStatus umi_editor_intel_search_scope_model_set_selected(UmiEditorIntelSearchScopeModel *model,bool selected);
int umi_editor_intel_search_scope_model_valid(const UmiEditorIntelSearchScopeModel *model);

#ifdef __cplusplus
}
#endif
#endif
