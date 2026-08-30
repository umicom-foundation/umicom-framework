/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_snapshot.h
 *
 * PURPOSE:
 *   Model refactor snapshot as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_SNAPSHOT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_SNAPSHOT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorSnapshot { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorSnapshot;
UmiStatus umi_editor_intel_refactor_snapshot_init(UmiEditorIntelRefactorSnapshot *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_snapshot_set_score(UmiEditorIntelRefactorSnapshot *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_snapshot_set_selected(UmiEditorIntelRefactorSnapshot *model,bool selected);
int umi_editor_intel_refactor_snapshot_valid(const UmiEditorIntelRefactorSnapshot *model);

#ifdef __cplusplus
}
#endif
#endif
