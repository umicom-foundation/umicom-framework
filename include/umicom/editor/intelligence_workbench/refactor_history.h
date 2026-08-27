/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_history.h
 *
 * PURPOSE:
 *   Model refactor history as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_HISTORY_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_HISTORY_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorHistory { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRefactorHistory;
UmiStatus umi_editor_intel_refactor_history_init(UmiEditorIntelRefactorHistory *model);
UmiStatus umi_editor_intel_refactor_history_add(UmiEditorIntelRefactorHistory *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_refactor_history_find(const UmiEditorIntelRefactorHistory *model,const char *id);
UmiStatus umi_editor_intel_refactor_history_clear(UmiEditorIntelRefactorHistory *model);
int umi_editor_intel_refactor_history_valid(const UmiEditorIntelRefactorHistory *model);

#ifdef __cplusplus
}
#endif
#endif
