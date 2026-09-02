/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_preview_model.h
 *
 * PURPOSE:
 *   Represent the preview state of a resolved code action.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PREVIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PREVIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action preview model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeActionPreviewModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionPreviewModel;
/**
 * Initialise editor intel code action preview model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_preview_model_init(UmiEditorIntelCodeActionPreviewModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel code action preview model set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_action_preview_model_set_score(UmiEditorIntelCodeActionPreviewModel *model,uint32_t score);
/**
 * Find editor intel code action preview model set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_intel_code_action_preview_model_set_selected(UmiEditorIntelCodeActionPreviewModel *model,bool selected);
/**
 * Check that editor intel code action preview model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_preview_model_valid(const UmiEditorIntelCodeActionPreviewModel *model);

#ifdef __cplusplus
}
#endif
#endif
