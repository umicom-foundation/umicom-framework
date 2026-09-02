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

/**
 * Represent the editor intel search result item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSearchResultItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSearchResultItem;
/**
 * Initialise editor intel search result item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_result_item_init(UmiEditorIntelSearchResultItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel search result item set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_search_result_item_set_score(UmiEditorIntelSearchResultItem *model,uint32_t score);
/**
 * Find editor intel search result item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_intel_search_result_item_set_selected(UmiEditorIntelSearchResultItem *model,bool selected);
/**
 * Check that editor intel search result item satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_search_result_item_valid(const UmiEditorIntelSearchResultItem *model);

#ifdef __cplusplus
}
#endif
#endif
