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

/**
 * Represent the editor intel refactor transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelRefactorTransaction { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorTransaction;
/**
 * Provide the editor intel refactor transaction begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_begin(UmiEditorIntelRefactorTransaction *transaction,const char *transaction_id,uint32_t total_operations);
/**
 * Perform editor intel refactor transaction record through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_transaction_record_apply(UmiEditorIntelRefactorTransaction *transaction);
/**
 * Provide the editor intel refactor transaction commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_commit(UmiEditorIntelRefactorTransaction *transaction);
/**
 * Provide the editor intel refactor transaction rollback operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_rollback(UmiEditorIntelRefactorTransaction *transaction);
/**
 * Check that editor intel refactor transaction satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_transaction_valid(const UmiEditorIntelRefactorTransaction *transaction);

#ifdef __cplusplus
}
#endif
#endif
