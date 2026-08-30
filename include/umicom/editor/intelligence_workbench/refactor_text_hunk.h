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

typedef struct UmiEditorIntelRefactorTextHunk { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorTextHunk;
UmiStatus umi_editor_intel_refactor_text_hunk_init(UmiEditorIntelRefactorTextHunk *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_text_hunk_set_score(UmiEditorIntelRefactorTextHunk *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_text_hunk_set_selected(UmiEditorIntelRefactorTextHunk *model,bool selected);
int umi_editor_intel_refactor_text_hunk_valid(const UmiEditorIntelRefactorTextHunk *model);

#ifdef __cplusplus
}
#endif
#endif
