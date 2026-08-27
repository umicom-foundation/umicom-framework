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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_RESULT_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelSearchResultGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelSearchResultGroup;
UmiStatus umi_editor_intel_search_result_group_init(UmiEditorIntelSearchResultGroup *model);
UmiStatus umi_editor_intel_search_result_group_add(UmiEditorIntelSearchResultGroup *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_search_result_group_find(const UmiEditorIntelSearchResultGroup *model,const char *id);
UmiStatus umi_editor_intel_search_result_group_clear(UmiEditorIntelSearchResultGroup *model);
int umi_editor_intel_search_result_group_valid(const UmiEditorIntelSearchResultGroup *model);

#ifdef __cplusplus
}
#endif
#endif
