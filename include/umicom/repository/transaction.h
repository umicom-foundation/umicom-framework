/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/transaction.h
 *
 * PURPOSE:
 *   Model staged native repository operations as auditable transactions.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_TRANSACTION_H
#define UMICOM_REPOSITORY_TRANSACTION_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named repository transaction action values accepted by this public contract.
 */
typedef enum UmiRepositoryTransactionAction {
    UMI_REPOSITORY_TRANSACTION_DISCOVER = 1,
    UMI_REPOSITORY_TRANSACTION_RESOLVE_HEAD = 2,
    UMI_REPOSITORY_TRANSACTION_VERIFY_GITLINK = 3,
    UMI_REPOSITORY_TRANSACTION_STAGE_GITLINK = 4,
    UMI_REPOSITORY_TRANSACTION_VERIFY_INDEX = 5
} UmiRepositoryTransactionAction;
/**
 * List the named repository transaction step state values accepted by this public
 * contract.
 */
typedef enum UmiRepositoryTransactionStepState {
    UMI_REPOSITORY_TRANSACTION_PENDING = 0,
    UMI_REPOSITORY_TRANSACTION_DONE = 1,
    UMI_REPOSITORY_TRANSACTION_FAILED = 2,
    UMI_REPOSITORY_TRANSACTION_SKIPPED = 3
} UmiRepositoryTransactionStepState;
/**
 * Represent the repository transaction step data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryTransactionStep {
    UmiRepositoryTransactionAction action;
    UmiRepositoryTransactionStepState state;
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    UmiStatus status;
} UmiRepositoryTransactionStep;
/**
 * Represent the repository transaction data shared with callers of this public contract.
 */
typedef struct UmiRepositoryTransaction {
    UmiRepositoryTransactionStep
        steps[UMI_REPOSITORY_CONTROL_TRANSACTION_CAPACITY];
    size_t count;
    uint64_t revision;
    int completed;
} UmiRepositoryTransaction;
/**
 * Initialise repository transaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_transaction_init(UmiRepositoryTransaction *transaction);
/**
 * Add repository transaction only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_repository_transaction_add(
    UmiRepositoryTransaction *transaction,
    UmiRepositoryTransactionAction action,
    const char *path);
/**
 * Provide the repository transaction mark operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_transaction_mark(
    UmiRepositoryTransaction *transaction,
    size_t index,
    UmiRepositoryTransactionStepState state,
    UmiStatus status);
/**
 * Provide the repository transaction successful operation used by this module and its
 * client applications.
 */
int umi_repository_transaction_successful(
    const UmiRepositoryTransaction *transaction);
#ifdef __cplusplus
}
#endif
#endif
