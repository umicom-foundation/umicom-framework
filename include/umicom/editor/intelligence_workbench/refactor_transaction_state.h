/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/refactor_transaction_state.h
 *
 * PURPOSE:
 *   Model refactor transaction state as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TRANSACTION_STATE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_REFACTOR_TRANSACTION_STATE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel refactor transaction state data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelRefactorTransactionState { char transaction_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t total_operations; uint32_t applied_operations; uint64_t revision; } UmiEditorIntelRefactorTransactionState;
/**
 * Provide the editor intel refactor transaction state begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_begin(UmiEditorIntelRefactorTransactionState *transaction,const char *transaction_id,uint32_t total_operations);
/**
 * Perform editor intel refactor transaction state record through the module contract so
 * client applications do not duplicate its policy.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_record_apply(UmiEditorIntelRefactorTransactionState *transaction);
/**
 * Provide the editor intel refactor transaction state commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_commit(UmiEditorIntelRefactorTransactionState *transaction);
/**
 * Provide the editor intel refactor transaction state rollback operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_refactor_transaction_state_rollback(UmiEditorIntelRefactorTransactionState *transaction);
/**
 * Check that editor intel refactor transaction state satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_transaction_state_valid(const UmiEditorIntelRefactorTransactionState *transaction);

#ifdef __cplusplus
}
#endif
#endif
