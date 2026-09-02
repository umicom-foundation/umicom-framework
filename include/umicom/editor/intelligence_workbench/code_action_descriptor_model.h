/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_descriptor_model.h
 *
 * PURPOSE:
 *   Model code action descriptor model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DESCRIPTOR_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_DESCRIPTOR_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action descriptor model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeActionDescriptorModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionDescriptorModel;
/**
 * Initialise editor intel code action descriptor model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_descriptor_model_init(UmiEditorIntelCodeActionDescriptorModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel code action descriptor model set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_action_descriptor_model_set_score(UmiEditorIntelCodeActionDescriptorModel *model,uint32_t score);
/**
 * Find editor intel code action descriptor model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_intel_code_action_descriptor_model_set_selected(UmiEditorIntelCodeActionDescriptorModel *model,bool selected);
/**
 * Check that editor intel code action descriptor model satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_code_action_descriptor_model_valid(const UmiEditorIntelCodeActionDescriptorModel *model);

#ifdef __cplusplus
}
#endif
#endif
