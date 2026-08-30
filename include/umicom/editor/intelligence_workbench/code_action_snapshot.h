/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_snapshot.h
 *
 * PURPOSE:
 *   Model code action snapshot as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SNAPSHOT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SNAPSHOT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionSnapshot { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionSnapshot;
UmiStatus umi_editor_intel_code_action_snapshot_init(UmiEditorIntelCodeActionSnapshot *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_snapshot_set_score(UmiEditorIntelCodeActionSnapshot *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_snapshot_set_selected(UmiEditorIntelCodeActionSnapshot *model,bool selected);
int umi_editor_intel_code_action_snapshot_valid(const UmiEditorIntelCodeActionSnapshot *model);

#ifdef __cplusplus
}
#endif
#endif
