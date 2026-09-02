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

/**
 * Represent the editor intel rename impact summary data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRenameImpactSummary { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelRenameImpactSummary;
/**
 * Initialise editor intel rename impact summary from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_rename_impact_summary_init(UmiEditorIntelRenameImpactSummary *model);
/**
 * Add editor intel rename impact summary only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_rename_impact_summary_add(UmiEditorIntelRenameImpactSummary *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel rename impact summary while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_rename_impact_summary_find(const UmiEditorIntelRenameImpactSummary *model,const char *id);
/**
 * Release or reset state held by editor intel rename impact summary so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_rename_impact_summary_clear(UmiEditorIntelRenameImpactSummary *model);
/**
 * Check that editor intel rename impact summary satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_rename_impact_summary_valid(const UmiEditorIntelRenameImpactSummary *model);

#ifdef __cplusplus
}
#endif
#endif
