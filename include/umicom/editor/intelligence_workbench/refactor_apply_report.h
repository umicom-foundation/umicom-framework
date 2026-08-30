/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_apply_report.h
 *
 * PURPOSE:
 *   Model refactor apply report as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_APPLY_REPORT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_APPLY_REPORT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorApplyReport { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorApplyReport;
UmiStatus umi_editor_intel_refactor_apply_report_begin(UmiEditorIntelRefactorApplyReport *transaction,const char *transaction_id,uint32_t total_operations);
UmiStatus umi_editor_intel_refactor_apply_report_record_apply(UmiEditorIntelRefactorApplyReport *transaction);
UmiStatus umi_editor_intel_refactor_apply_report_commit(UmiEditorIntelRefactorApplyReport *transaction);
UmiStatus umi_editor_intel_refactor_apply_report_rollback(UmiEditorIntelRefactorApplyReport *transaction);
int umi_editor_intel_refactor_apply_report_valid(const UmiEditorIntelRefactorApplyReport *transaction);

#ifdef __cplusplus
}
#endif
#endif
