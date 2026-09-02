/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/transaction.h
 *
 * PURPOSE:
 *   Apply several layout operations atomically with optimistic revision checking, validation, rollback evidence and immutable before/after state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_TRANSACTION_H
#define UMICOM_WORKBENCH_LAYOUT_TRANSACTION_H

#include "umicom/workbench_layout/operation.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench layout transaction state values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchLayoutTransactionState {
    UMI_WORKBENCH_LAYOUT_TRANSACTION_EMPTY = 1,
    UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN = 2,
    UMI_WORKBENCH_LAYOUT_TRANSACTION_COMMITTED = 3,
    UMI_WORKBENCH_LAYOUT_TRANSACTION_ROLLED_BACK = 4,
    UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED = 5
} UmiWorkbenchLayoutTransactionState;

/**
 * Represent the workbench layout transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutTransaction {
    uint32_t structure_size;
    char transaction_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutDocument before;
    UmiWorkbenchLayoutDocument working;
    UmiWorkbenchLayoutOperation
        operations[UMI_WORKBENCH_LAYOUT_MAX_OPERATIONS];
    size_t operation_count;
    UmiWorkbenchLayoutTransactionState state;
    uint64_t expected_revision;
    bool validate_on_commit;
} UmiWorkbenchLayoutTransaction;

/**
 * Represent the workbench layout transaction result data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutTransactionResult {
    uint32_t structure_size;
    UmiStatus status;
    UmiWorkbenchLayoutTransactionState state;
    size_t applied_operation_count;
    size_t failed_operation_index;
    UmiWorkbenchLayoutValidationReport validation;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutTransactionResult;

/**
 * Initialise workbench layout transaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_transaction_init(
    UmiWorkbenchLayoutTransaction *transaction);

/**
 * Provide the workbench layout transaction begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_begin(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutDocument *document,
    const char *transaction_id,
    const char *actor_id,
    const char *correlation_id,
    uint64_t expected_revision);

/**
 * Add workbench layout transaction only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_layout_transaction_append(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation);

/**
 * Perform workbench layout transaction through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_transaction_apply(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result);

/**
 * Provide the workbench layout transaction commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_commit(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutValidationOptions *validation_options,
    UmiWorkbenchLayoutTransactionResult *out_result);

/**
 * Provide the workbench layout transaction rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_rollback(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutTransactionResult *out_result);

/**
 * Provide the workbench layout transaction preview operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutDocument *
umi_workbench_layout_transaction_preview(
    const UmiWorkbenchLayoutTransaction *transaction);

#ifdef __cplusplus
}
#endif

#endif
