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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DIAGNOSTIC_BINDING_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DIAGNOSTIC_BINDING_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action diagnostic binding data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorIntelCodeActionDiagnosticBinding { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionDiagnosticBinding;
/**
 * Initialise editor intel code action diagnostic binding from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_diagnostic_binding_init(UmiEditorIntelCodeActionDiagnosticBinding *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel code action diagnostic binding set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_action_diagnostic_binding_set_score(UmiEditorIntelCodeActionDiagnosticBinding *model,uint32_t score);
/**
 * Find editor intel code action diagnostic binding set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_intel_code_action_diagnostic_binding_set_selected(UmiEditorIntelCodeActionDiagnosticBinding *model,bool selected);
/**
 * Check that editor intel code action diagnostic binding satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_code_action_diagnostic_binding_valid(const UmiEditorIntelCodeActionDiagnosticBinding *model);

#ifdef __cplusplus
}
#endif
#endif
