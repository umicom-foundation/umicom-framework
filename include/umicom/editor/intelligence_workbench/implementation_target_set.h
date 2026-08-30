/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/implementation_target_set.h
 *
 * PURPOSE:
 *   Model implementation target set as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_IMPLEMENTATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_IMPLEMENTATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelImplementationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelImplementationTargetSet;
UmiStatus umi_editor_intel_implementation_target_set_init(UmiEditorIntelImplementationTargetSet *model);
UmiStatus umi_editor_intel_implementation_target_set_add(UmiEditorIntelImplementationTargetSet *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_implementation_target_set_find(const UmiEditorIntelImplementationTargetSet *model,const char *id);
UmiStatus umi_editor_intel_implementation_target_set_clear(UmiEditorIntelImplementationTargetSet *model);
int umi_editor_intel_implementation_target_set_valid(const UmiEditorIntelImplementationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
