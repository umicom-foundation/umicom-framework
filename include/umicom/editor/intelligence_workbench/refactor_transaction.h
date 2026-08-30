/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_transaction.h
 *
 * PURPOSE:
 *   Coordinate begin, apply, commit and rollback state for one refactor.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TRANSACTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TRANSACTION_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelRefactorTransaction { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorTransaction;
UmiStatus umi_editor_intel_refactor_transaction_begin(UmiEditorIntelRefactorTransaction *transaction,const char *transaction_id,uint32_t total_operations);
UmiStatus umi_editor_intel_refactor_transaction_record_apply(UmiEditorIntelRefactorTransaction *transaction);
UmiStatus umi_editor_intel_refactor_transaction_commit(UmiEditorIntelRefactorTransaction *transaction);
UmiStatus umi_editor_intel_refactor_transaction_rollback(UmiEditorIntelRefactorTransaction *transaction);
int umi_editor_intel_refactor_transaction_valid(const UmiEditorIntelRefactorTransaction *transaction);

#ifdef __cplusplus
}
#endif
#endif
