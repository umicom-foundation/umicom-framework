/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_conformance.h
 *
 * PURPOSE:
 *   Represent conformance evidence for search and refactoring orchestration.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CONFORMANCE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_CONFORMANCE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorConformance { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelRefactorConformance;
UmiStatus umi_editor_intel_refactor_conformance_init(UmiEditorIntelRefactorConformance *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_refactor_conformance_set_score(UmiEditorIntelRefactorConformance *model,uint32_t score);
UmiStatus umi_editor_intel_refactor_conformance_set_selected(UmiEditorIntelRefactorConformance *model,bool selected);
int umi_editor_intel_refactor_conformance_valid(const UmiEditorIntelRefactorConformance *model);

#ifdef __cplusplus
}
#endif
#endif
