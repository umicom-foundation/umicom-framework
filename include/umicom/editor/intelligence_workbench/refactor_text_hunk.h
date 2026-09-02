/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_text_hunk.h
 *
 * PURPOSE:
 *   Model refactor text hunk as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TEXT_HUNK_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TEXT_HUNK_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor text hunk data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorTextHunk { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorTextHunk;
/**
 * Initialise editor intel refactor text hunk from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_init(UmiEditorIntelRefactorTextHunk *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel refactor text hunk set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_set_score(UmiEditorIntelRefactorTextHunk *model,uint32_t score);
/**
 * Find editor intel refactor text hunk set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_set_selected(UmiEditorIntelRefactorTextHunk *model,bool selected);
/**
 * Check that editor intel refactor text hunk satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_refactor_text_hunk_valid(const UmiEditorIntelRefactorTextHunk *model);

#ifdef __cplusplus
}
#endif
#endif
