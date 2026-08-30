/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/rename_impact_summary.h
 *
 * PURPOSE:
 *   Aggregate rename impacts across files before the change is applied.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_SUMMARY_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RENAME_IMPACT_SUMMARY_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRenameImpactSummary { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRenameImpactSummary;
UmiStatus umi_editor_intel_rename_impact_summary_init(UmiEditorIntelRenameImpactSummary *model);
UmiStatus umi_editor_intel_rename_impact_summary_add(UmiEditorIntelRenameImpactSummary *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_rename_impact_summary_find(const UmiEditorIntelRenameImpactSummary *model,const char *id);
UmiStatus umi_editor_intel_rename_impact_summary_clear(UmiEditorIntelRenameImpactSummary *model);
int umi_editor_intel_rename_impact_summary_valid(const UmiEditorIntelRenameImpactSummary *model);

#ifdef __cplusplus
}
#endif
#endif
