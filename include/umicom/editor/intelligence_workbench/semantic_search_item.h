/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/semantic_search_item.h
 *
 * PURPOSE:
 *   Represent a semantically ranked search match without renderer ownership.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEMANTIC_SEARCH_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEMANTIC_SEARCH_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSemanticSearchItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSemanticSearchItem;
UmiStatus umi_editor_intel_semantic_search_item_init(UmiEditorIntelSemanticSearchItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_semantic_search_item_set_score(UmiEditorIntelSemanticSearchItem *model,uint32_t score);
UmiStatus umi_editor_intel_semantic_search_item_set_selected(UmiEditorIntelSemanticSearchItem *model,bool selected);
int umi_editor_intel_semantic_search_item_valid(const UmiEditorIntelSemanticSearchItem *model);

#ifdef __cplusplus
}
#endif
#endif
