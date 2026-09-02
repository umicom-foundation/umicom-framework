/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_file_change.h
 *
 * PURPOSE:
 *   Model refactor file change as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_FILE_CHANGE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_FILE_CHANGE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor file change data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorFileChange { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorFileChange;
/**
 * Initialise editor intel refactor file change from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_file_change_init(UmiEditorIntelRefactorFileChange *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel refactor file change set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_file_change_set_score(UmiEditorIntelRefactorFileChange *model,uint32_t score);
/**
 * Find editor intel refactor file change set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_refactor_file_change_set_selected(UmiEditorIntelRefactorFileChange *model,bool selected);
/**
 * Check that editor intel refactor file change satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_file_change_valid(const UmiEditorIntelRefactorFileChange *model);

#ifdef __cplusplus
}
#endif
#endif
