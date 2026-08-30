/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/declaration_target_set.h
 *
 * PURPOSE:
 *   Model declaration target set as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_DECLARATION_TARGET_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_DECLARATION_TARGET_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelDeclarationTargetSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelDeclarationTargetSet;
UmiStatus umi_editor_intel_declaration_target_set_init(UmiEditorIntelDeclarationTargetSet *model);
UmiStatus umi_editor_intel_declaration_target_set_add(UmiEditorIntelDeclarationTargetSet *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_declaration_target_set_find(const UmiEditorIntelDeclarationTargetSet *model,const char *id);
UmiStatus umi_editor_intel_declaration_target_set_clear(UmiEditorIntelDeclarationTargetSet *model);
int umi_editor_intel_declaration_target_set_valid(const UmiEditorIntelDeclarationTargetSet *model);

#ifdef __cplusplus
}
#endif
#endif
