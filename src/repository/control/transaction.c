/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/transaction.c
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
#include "umicom/repository/transaction.h"

#include <string.h>

/*
 * Initialise repository transaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_transaction_init(UmiRepositoryTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return;
    (void)memset(transaction, 0, sizeof(*transaction));
    transaction->revision = 1U;
}

/*
 * Add repository transaction only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_repository_transaction_add(
    UmiRepositoryTransaction *transaction,
    UmiRepositoryTransactionAction action,
    const char *path)
{
    UmiRepositoryTransactionStep *step;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || path == NULL || path[0] == '\0' ||
        action < UMI_REPOSITORY_TRANSACTION_DISCOVER ||
        action > UMI_REPOSITORY_TRANSACTION_VERIFY_INDEX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (transaction->completed) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (transaction->count >= UMI_REPOSITORY_CONTROL_TRANSACTION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    length = strlen(path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > UMI_REPOSITORY_CONTROL_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    step = &transaction->steps[transaction->count++];
    step->action = action;
    step->state = UMI_REPOSITORY_TRANSACTION_PENDING;
    step->status = UMI_STATUS_OK;
    (void)memcpy(step->path, path, length + 1U);
    transaction->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository transaction mark operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_transaction_mark(
    UmiRepositoryTransaction *transaction,
    size_t index,
    UmiRepositoryTransactionStepState state,
    UmiStatus status)
{
    size_t cursor;
    int complete = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || index >= transaction->count ||
        state < UMI_REPOSITORY_TRANSACTION_PENDING ||
        state > UMI_REPOSITORY_TRANSACTION_SKIPPED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    transaction->steps[index].state = state;
    transaction->steps[index].status = status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = 0U; cursor < transaction->count; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (transaction->steps[cursor].state ==
            UMI_REPOSITORY_TRANSACTION_PENDING) {
            complete = 0;
            break;
        }
    }
    transaction->completed = complete;
    transaction->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository transaction successful operation used by this module and its
 * client applications.
 */
int umi_repository_transaction_successful(
    const UmiRepositoryTransaction *transaction)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || !transaction->completed) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < transaction->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (transaction->steps[index].state ==
            UMI_REPOSITORY_TRANSACTION_FAILED) {
            return 0;
        }
    }
    return 1;
}
