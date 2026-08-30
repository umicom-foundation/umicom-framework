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

typedef struct UmiEditorIntelRefactorFileChange { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorFileChange;
UmiStatus umi_editor_intel_refactor_file_change_init(UmiEditorIntelRefactorFileChange *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_file_change_set_score(UmiEditorIntelRefactorFileChange *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_file_change_set_selected(UmiEditorIntelRefactorFileChange *model,bool selected);
int umi_editor_intel_refactor_file_change_valid(const UmiEditorIntelRefactorFileChange *model);

#ifdef __cplusplus
}
#endif
#endif
