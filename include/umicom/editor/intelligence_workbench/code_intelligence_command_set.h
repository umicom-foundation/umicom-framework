/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_intelligence_command_set.h
 *
 * PURPOSE:
 *   Model code intelligence command set as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_COMMAND_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_COMMAND_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeIntelligenceCommandSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCodeIntelligenceCommandSet;
UmiStatus umi_editor_intel_code_intelligence_command_set_init(UmiEditorIntelCodeIntelligenceCommandSet *model);
UmiStatus umi_editor_intel_code_intelligence_command_set_add(UmiEditorIntelCodeIntelligenceCommandSet *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_code_intelligence_command_set_find(const UmiEditorIntelCodeIntelligenceCommandSet *model,const char *id);
UmiStatus umi_editor_intel_code_intelligence_command_set_clear(UmiEditorIntelCodeIntelligenceCommandSet *model);
int umi_editor_intel_code_intelligence_command_set_valid(const UmiEditorIntelCodeIntelligenceCommandSet *model);

#ifdef __cplusplus
}
#endif
#endif
