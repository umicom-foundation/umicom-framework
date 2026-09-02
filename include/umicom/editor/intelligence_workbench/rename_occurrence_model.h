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

/**
 * Represent the editor intel rename occurrence model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelRenameOccurrenceModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRenameOccurrenceModel;
/**
 * Initialise editor intel rename occurrence model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_init(UmiEditorIntelRenameOccurrenceModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel rename occurrence model set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_set_score(UmiEditorIntelRenameOccurrenceModel *model,uint32_t score);
/**
 * Find editor intel rename occurrence model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_set_selected(UmiEditorIntelRenameOccurrenceModel *model,bool selected);
/**
 * Check that editor intel rename occurrence model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_rename_occurrence_model_valid(const UmiEditorIntelRenameOccurrenceModel *model);

#ifdef __cplusplus
}
#endif
#endif
