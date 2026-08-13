/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/transaction.c
 *
 * PURPOSE:
 *   Implement recoverable distribution transaction plans, valid state changes
 *   and bounded operational history.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/transaction.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDistributionTransactionLog {
    UmiDistributionTransaction items[UMI_DISTRIBUTION_TRANSACTION_CAPACITY];
    size_t count;
    uint64_t revision;
};

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

static UmiStatus add_step(UmiDistributionTransaction *transaction,
                          const char *step)
{
    if (transaction->step_count >= UMI_DISTRIBUTION_TRANSACTION_STEP_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!copy_text(transaction->steps[transaction->step_count],
                   sizeof(transaction->steps[0]), step)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++transaction->step_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_transaction_plan(
    UmiDistributionTransaction *transaction,
    const char *transaction_id,
    UmiDistributionTransactionAction action,
    const char *product_id,
    const char *release_id,
    UmiVersion from_version,
    UmiVersion to_version,
    const char *install_root,
    uint64_t timestamp_ms)
{
    UmiStatus status;
    if (transaction == NULL || transaction_id == NULL || product_id == NULL ||
        release_id == NULL || install_root == NULL ||
        action < UMI_DISTRIBUTION_INSTALL || action > UMI_DISTRIBUTION_UNINSTALL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(transaction, 0, sizeof(*transaction));
    if (!copy_text(transaction->transaction_id, sizeof(transaction->transaction_id),
                   transaction_id) ||
        !copy_text(transaction->product_id, sizeof(transaction->product_id), product_id) ||
        !copy_text(transaction->release_id, sizeof(transaction->release_id), release_id) ||
        !copy_text(transaction->install_root, sizeof(transaction->install_root), install_root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    transaction->action = action;
    transaction->state = UMI_DISTRIBUTION_TRANSACTION_PLANNED;
    transaction->from_version = from_version;
    transaction->to_version = to_version;
    transaction->reversible = action != UMI_DISTRIBUTION_UNINSTALL;
    transaction->requires_restart = 1;
    transaction->created_at_ms = timestamp_ms;
    transaction->updated_at_ms = timestamp_ms;
    transaction->revision = 1U;
    status = add_step(transaction, "reserve transaction and installation lock");
    if (status == UMI_STATUS_OK) status = add_step(
        transaction, "stage package outside the active installation");
    if (status == UMI_STATUS_OK) status = add_step(
        transaction, "verify checksum, signature, SBOM, licences and provenance");
    if (status == UMI_STATUS_OK && action != UMI_DISTRIBUTION_INSTALL) {
        status = add_step(transaction, "create recoverable generation checkpoint");
    }
    if (status == UMI_STATUS_OK) status = add_step(
        transaction, action == UMI_DISTRIBUTION_UNINSTALL
            ? "remove the selected installed generation"
            : "activate the staged generation atomically");
    if (status == UMI_STATUS_OK) status = add_step(
        transaction, "run health verification and record installation state");
    if (status == UMI_STATUS_OK) status = add_step(
        transaction, "release transaction lock and publish audit result");
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(transaction->message, sizeof(transaction->message),
                   "%s transaction planned",
                   umi_distribution_transaction_action_text(action));
    return UMI_STATUS_OK;
}

int umi_distribution_transaction_transition_valid(
    UmiDistributionTransactionState current,
    UmiDistributionTransactionState next_state)
{
    if (next_state == UMI_DISTRIBUTION_TRANSACTION_FAILED ||
        next_state == UMI_DISTRIBUTION_TRANSACTION_CANCELLED) {
        return current != UMI_DISTRIBUTION_TRANSACTION_COMPLETED &&
               current != UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK;
    }
    switch (current) {
        case UMI_DISTRIBUTION_TRANSACTION_PLANNED:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_STAGING;
        case UMI_DISTRIBUTION_TRANSACTION_STAGING:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_VERIFYING ||
                   next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK;
        case UMI_DISTRIBUTION_TRANSACTION_VERIFYING:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_READY ||
                   next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK;
        case UMI_DISTRIBUTION_TRANSACTION_READY:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_APPLYING;
        case UMI_DISTRIBUTION_TRANSACTION_APPLYING:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_COMPLETED ||
                   next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK;
        case UMI_DISTRIBUTION_TRANSACTION_COMPLETED:
        case UMI_DISTRIBUTION_TRANSACTION_FAILED:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK;
        case UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK:
            return next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK;
        default:
            return 0;
    }
}

UmiStatus umi_distribution_transaction_transition(
    UmiDistributionTransaction *transaction,
    UmiDistributionTransactionState next_state,
    const char *message,
    uint64_t timestamp_ms)
{
    if (transaction == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_distribution_transaction_transition_valid(transaction->state, next_state)) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (next_state == UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK &&
        !transaction->reversible) return UMI_STATUS_PERMISSION_DENIED;
    transaction->state = next_state;
    transaction->updated_at_ms = timestamp_ms;
    if (transaction->current_step < transaction->step_count &&
        next_state != UMI_DISTRIBUTION_TRANSACTION_FAILED &&
        next_state != UMI_DISTRIBUTION_TRANSACTION_CANCELLED) {
        ++transaction->current_step;
    }
    if (!copy_text(transaction->message, sizeof(transaction->message), message)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++transaction->revision;
    return UMI_STATUS_OK;
}

const char *umi_distribution_transaction_action_text(
    UmiDistributionTransactionAction action)
{
    switch (action) {
        case UMI_DISTRIBUTION_INSTALL: return "install";
        case UMI_DISTRIBUTION_UPDATE: return "update";
        case UMI_DISTRIBUTION_ROLLBACK: return "rollback";
        case UMI_DISTRIBUTION_UNINSTALL: return "uninstall";
        default: return "unknown";
    }
}

const char *umi_distribution_transaction_state_text(
    UmiDistributionTransactionState state)
{
    switch (state) {
        case UMI_DISTRIBUTION_TRANSACTION_PLANNED: return "planned";
        case UMI_DISTRIBUTION_TRANSACTION_STAGING: return "staging";
        case UMI_DISTRIBUTION_TRANSACTION_VERIFYING: return "verifying";
        case UMI_DISTRIBUTION_TRANSACTION_READY: return "ready";
        case UMI_DISTRIBUTION_TRANSACTION_APPLYING: return "applying";
        case UMI_DISTRIBUTION_TRANSACTION_COMPLETED: return "completed";
        case UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK: return "rolling-back";
        case UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK: return "rolled-back";
        case UMI_DISTRIBUTION_TRANSACTION_FAILED: return "failed";
        case UMI_DISTRIBUTION_TRANSACTION_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}

static size_t log_find(const UmiDistributionTransactionLog *log,
                       const char *transaction_id)
{
    size_t index;
    if (log == NULL || transaction_id == NULL) return SIZE_MAX;
    for (index = 0U; index < log->count; ++index) {
        if (strcmp(log->items[index].transaction_id, transaction_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_distribution_transaction_log_create(
    UmiDistributionTransactionLog **out_log)
{
    UmiDistributionTransactionLog *log;
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL;
    log = (UmiDistributionTransactionLog *)calloc(1U, sizeof(*log));
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->revision = 1U;
    *out_log = log;
    return UMI_STATUS_OK;
}

void umi_distribution_transaction_log_destroy(UmiDistributionTransactionLog *log)
{
    free(log);
}

UmiStatus umi_distribution_transaction_log_upsert(
    UmiDistributionTransactionLog *log,
    const UmiDistributionTransaction *transaction)
{
    size_t index;
    if (log == NULL || transaction == NULL || transaction->transaction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = log_find(log, transaction->transaction_id);
    if (index == SIZE_MAX) {
        if (log->count >= UMI_DISTRIBUTION_TRANSACTION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = log->count++;
    }
    log->items[index] = *transaction;
    ++log->revision;
    log->items[index].revision = log->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_transaction_log_find(
    const UmiDistributionTransactionLog *log,
    const char *transaction_id,
    UmiDistributionTransaction *out_transaction)
{
    size_t index;
    if (log == NULL || transaction_id == NULL || out_transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = log_find(log, transaction_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_transaction = log->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_transaction_log_at(
    const UmiDistributionTransactionLog *log,
    size_t index,
    UmiDistributionTransaction *out_transaction)
{
    if (log == NULL || out_transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= log->count) return UMI_STATUS_NOT_FOUND;
    *out_transaction = log->items[index];
    return UMI_STATUS_OK;
}

size_t umi_distribution_transaction_log_count(
    const UmiDistributionTransactionLog *log)
{
    return log != NULL ? log->count : 0U;
}
