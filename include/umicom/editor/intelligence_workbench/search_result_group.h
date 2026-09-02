/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_result_group.h
 *
 * PURPOSE:
 *   Model search result group as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel search result group data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSearchResultGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelSearchResultGroup;
/**
 * Initialise editor intel search result group from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_result_group_init(UmiEditorIntelSearchResultGroup *model);
/**
 * Add editor intel search result group only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_search_result_group_add(UmiEditorIntelSearchResultGroup *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel search result group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_search_result_group_find(const UmiEditorIntelSearchResultGroup *model,const char *id);
/**
 * Release or reset state held by editor intel search result group so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_intel_search_result_group_clear(UmiEditorIntelSearchResultGroup *model);
/**
 * Check that editor intel search result group satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_search_result_group_valid(const UmiEditorIntelSearchResultGroup *model);

#ifdef __cplusplus
}
#endif
#endif
