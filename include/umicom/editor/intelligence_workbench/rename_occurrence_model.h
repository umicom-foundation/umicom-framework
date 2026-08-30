/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/rename_occurrence_model.h
 *
 * PURPOSE:
 *   Model rename occurrence model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_OCCURRENCE_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_OCCURRENCE_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRenameOccurrenceModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRenameOccurrenceModel;
UmiStatus umi_editor_intel_rename_occurrence_model_init(UmiEditorIntelRenameOccurrenceModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_rename_occurrence_model_set_score(UmiEditorIntelRenameOccurrenceModel *model,uint32_t score);
UmiStatus umi_editor_intel_rename_occurrence_model_set_selected(UmiEditorIntelRenameOccurrenceModel *model,bool selected);
int umi_editor_intel_rename_occurrence_model_valid(const UmiEditorIntelRenameOccurrenceModel *model);

#ifdef __cplusplus
}
#endif
#endif
