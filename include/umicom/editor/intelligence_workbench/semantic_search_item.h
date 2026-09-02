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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEMANTIC_SEARCH_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEMANTIC_SEARCH_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel semantic search item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSemanticSearchItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelSemanticSearchItem;
/**
 * Initialise editor intel semantic search item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_semantic_search_item_init(UmiEditorIntelSemanticSearchItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel semantic search item set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_semantic_search_item_set_score(UmiEditorIntelSemanticSearchItem *model,uint32_t score);
/**
 * Find editor intel semantic search item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_semantic_search_item_set_selected(UmiEditorIntelSemanticSearchItem *model,bool selected);
/**
 * Check that editor intel semantic search item satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_semantic_search_item_valid(const UmiEditorIntelSemanticSearchItem *model);

#ifdef __cplusplus
}
#endif
#endif
