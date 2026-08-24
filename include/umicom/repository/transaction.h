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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_TRANSACTION_H
#define UMICOM_REPOSITORY_TRANSACTION_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiRepositoryTransactionAction {
    UMI_REPOSITORY_TRANSACTION_DISCOVER = 1,
    UMI_REPOSITORY_TRANSACTION_RESOLVE_HEAD = 2,
    UMI_REPOSITORY_TRANSACTION_VERIFY_GITLINK = 3,
    UMI_REPOSITORY_TRANSACTION_STAGE_GITLINK = 4,
    UMI_REPOSITORY_TRANSACTION_VERIFY_INDEX = 5
} UmiRepositoryTransactionAction;
typedef enum UmiRepositoryTransactionStepState {
    UMI_REPOSITORY_TRANSACTION_PENDING = 0,
    UMI_REPOSITORY_TRANSACTION_DONE = 1,
    UMI_REPOSITORY_TRANSACTION_FAILED = 2,
    UMI_REPOSITORY_TRANSACTION_SKIPPED = 3
} UmiRepositoryTransactionStepState;
typedef struct UmiRepositoryTransactionStep {
    UmiRepositoryTransactionAction action;
    UmiRepositoryTransactionStepState state;
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    UmiStatus status;
} UmiRepositoryTransactionStep;
typedef struct UmiRepositoryTransaction {
    UmiRepositoryTransactionStep
        steps[UMI_REPOSITORY_CONTROL_TRANSACTION_CAPACITY];
    size_t count;
    uint64_t revision;
    int completed;
} UmiRepositoryTransaction;
void umi_repository_transaction_init(UmiRepositoryTransaction *transaction);
UmiStatus umi_repository_transaction_add(
    UmiRepositoryTransaction *transaction,
    UmiRepositoryTransactionAction action,
    const char *path);
UmiStatus umi_repository_transaction_mark(
    UmiRepositoryTransaction *transaction,
    size_t index,
    UmiRepositoryTransactionStepState state,
    UmiStatus status);
int umi_repository_transaction_successful(
    const UmiRepositoryTransaction *transaction);
#ifdef __cplusplus
}
#endif
#endif
