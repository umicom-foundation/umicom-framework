/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_intelligence_keymap_context.h
 *
 * PURPOSE:
 *   Model code intelligence keymap context as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_KEYMAP_CONTEXT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_KEYMAP_CONTEXT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeIntelligenceKeymapContext { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCodeIntelligenceKeymapContext;
UmiStatus umi_editor_intel_code_intelligence_keymap_context_init(UmiEditorIntelCodeIntelligenceKeymapContext *model);
UmiStatus umi_editor_intel_code_intelligence_keymap_context_add(UmiEditorIntelCodeIntelligenceKeymapContext *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_code_intelligence_keymap_context_find(const UmiEditorIntelCodeIntelligenceKeymapContext *model,const char *id);
UmiStatus umi_editor_intel_code_intelligence_keymap_context_clear(UmiEditorIntelCodeIntelligenceKeymapContext *model);
int umi_editor_intel_code_intelligence_keymap_context_valid(const UmiEditorIntelCodeIntelligenceKeymapContext *model);

#ifdef __cplusplus
}
#endif
#endif
