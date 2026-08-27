/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_rollback_report.h
 *
 * PURPOSE:
 *   Model refactor rollback report as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_ROLLBACK_REPORT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_ROLLBACK_REPORT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorRollbackReport { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorRollbackReport;
UmiStatus umi_editor_intel_refactor_rollback_report_begin(UmiEditorIntelRefactorRollbackReport *transaction,const char *transaction_id,uint32_t total_operations);
UmiStatus umi_editor_intel_refactor_rollback_report_record_apply(UmiEditorIntelRefactorRollbackReport *transaction);
UmiStatus umi_editor_intel_refactor_rollback_report_commit(UmiEditorIntelRefactorRollbackReport *transaction);
UmiStatus umi_editor_intel_refactor_rollback_report_rollback(UmiEditorIntelRefactorRollbackReport *transaction);
int umi_editor_intel_refactor_rollback_report_valid(const UmiEditorIntelRefactorRollbackReport *transaction);

#ifdef __cplusplus
}
#endif
#endif
