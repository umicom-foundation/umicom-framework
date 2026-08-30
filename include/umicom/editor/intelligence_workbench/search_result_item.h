/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_result_item.h
 *
 * PURPOSE:
 *   Model search result item as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchResultItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchResultItem;
UmiStatus umi_editor_intel_search_result_item_init(UmiEditorIntelSearchResultItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_search_result_item_set_score(UmiEditorIntelSearchResultItem *model,uint32_t score);
UmiStatus umi_editor_intel_search_result_item_set_selected(UmiEditorIntelSearchResultItem *model,bool selected);
int umi_editor_intel_search_result_item_valid(const UmiEditorIntelSearchResultItem *model);

#ifdef __cplusplus
}
#endif
#endif
