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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_KEYMAP_CONTEXT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_KEYMAP_CONTEXT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code intelligence keymap context data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorIntelCodeIntelligenceKeymapContext { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCodeIntelligenceKeymapContext;
/**
 * Initialise editor intel code intelligence keymap context from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_code_intelligence_keymap_context_init(UmiEditorIntelCodeIntelligenceKeymapContext *model);
/**
 * Add editor intel code intelligence keymap context only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_intel_code_intelligence_keymap_context_add(UmiEditorIntelCodeIntelligenceKeymapContext *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel code intelligence keymap context while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_code_intelligence_keymap_context_find(const UmiEditorIntelCodeIntelligenceKeymapContext *model,const char *id);
/**
 * Release or reset state held by editor intel code intelligence keymap context so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_code_intelligence_keymap_context_clear(UmiEditorIntelCodeIntelligenceKeymapContext *model);
/**
 * Check that editor intel code intelligence keymap context satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_code_intelligence_keymap_context_valid(const UmiEditorIntelCodeIntelligenceKeymapContext *model);

#ifdef __cplusplus
}
#endif
#endif
