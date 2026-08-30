/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/navigation_target_set.h
 *
 * PURPOSE:
 *   Deduplicate and order navigation targets before presentation.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_NAVIGATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelNavigationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelNavigationTargetSet;
UmiStatus umi_editor_intel_navigation_target_set_init(UmiEditorIntelNavigationTargetSet *model);
UmiStatus umi_editor_intel_navigation_target_set_add(UmiEditorIntelNavigationTargetSet *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_navigation_target_set_find(const UmiEditorIntelNavigationTargetSet *model,const char *id);
UmiStatus umi_editor_intel_navigation_target_set_clear(UmiEditorIntelNavigationTargetSet *model);
int umi_editor_intel_navigation_target_set_valid(const UmiEditorIntelNavigationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
