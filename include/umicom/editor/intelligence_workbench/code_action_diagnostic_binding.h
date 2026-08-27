/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_diagnostic_binding.h
 *
 * PURPOSE:
 *   Model code action diagnostic binding as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DIAGNOSTIC_BINDING_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DIAGNOSTIC_BINDING_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionDiagnosticBinding { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionDiagnosticBinding;
UmiStatus umi_editor_intel_code_action_diagnostic_binding_init(UmiEditorIntelCodeActionDiagnosticBinding *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_diagnostic_binding_set_score(UmiEditorIntelCodeActionDiagnosticBinding *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_diagnostic_binding_set_selected(UmiEditorIntelCodeActionDiagnosticBinding *model,bool selected);
int umi_editor_intel_code_action_diagnostic_binding_valid(const UmiEditorIntelCodeActionDiagnosticBinding *model);

#ifdef __cplusplus
}
#endif
#endif
