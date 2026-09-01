/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/transaction.h
 *
 * PURPOSE:
 *   Model recoverable install, update, rollback and uninstall transactions as
 *   explicit state machines with inspectable steps.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/*
 * Adapters execute steps one at a time and record transitions.  Framework never
 * hides destructive filesystem work behind a catalogue query or UI refresh.
 */
#ifndef UMICOM_DISTRIBUTION_TRANSACTION_H
#define UMICOM_DISTRIBUTION_TRANSACTION_H

#include "umicom/distribution/policy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DISTRIBUTION_TRANSACTION_CAPACITY 256U
#define UMI_DISTRIBUTION_TRANSACTION_STEP_MAX 16U

typedef enum UmiDistributionTransactionAction {
    UMI_DISTRIBUTION_INSTALL = 1,
    UMI_DISTRIBUTION_UPDATE = 2,
    UMI_DISTRIBUTION_ROLLBACK = 3,
    UMI_DISTRIBUTION_UNINSTALL = 4
} UmiDistributionTransactionAction;

typedef enum UmiDistributionTransactionState {
    UMI_DISTRIBUTION_TRANSACTION_PLANNED = 1,
    UMI_DISTRIBUTION_TRANSACTION_STAGING = 2,
    UMI_DISTRIBUTION_TRANSACTION_VERIFYING = 3,
    UMI_DISTRIBUTION_TRANSACTION_READY = 4,
    UMI_DISTRIBUTION_TRANSACTION_APPLYING = 5,
    UMI_DISTRIBUTION_TRANSACTION_COMPLETED = 6,
    UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK = 7,
    UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK = 8,
    UMI_DISTRIBUTION_TRANSACTION_FAILED = 9,
    UMI_DISTRIBUTION_TRANSACTION_CANCELLED = 10
} UmiDistributionTransactionState;

typedef struct UmiDistributionTransaction {
    char transaction_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char product_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char release_id[UMI_DISTRIBUTION_ID_CAPACITY];
    UmiDistributionTransactionAction action;
    UmiDistributionTransactionState state;
    UmiVersion from_version;
    UmiVersion to_version;
    char install_root[UMI_DELIVERY_PATH_CAPACITY];
    char steps[UMI_DISTRIBUTION_TRANSACTION_STEP_MAX][128];
    size_t step_count;
    size_t current_step;
    int requires_restart;
    int reversible;
    uint64_t created_at_ms;
    uint64_t updated_at_ms;
    char message[256];
    uint64_t revision;
} UmiDistributionTransaction;

typedef struct UmiDistributionTransactionLog UmiDistributionTransactionLog;

UmiStatus umi_distribution_transaction_plan(
    UmiDistributionTransaction *transaction,
    const char *transaction_id,
    UmiDistributionTransactionAction action,
    const char *product_id,
    const char *release_id,
    UmiVersion from_version,
    UmiVersion to_version,
    const char *install_root,
    uint64_t timestamp_ms);
UmiStatus umi_distribution_transaction_transition(
    UmiDistributionTransaction *transaction,
    UmiDistributionTransactionState next_state,
    const char *message,
    uint64_t timestamp_ms);
int umi_distribution_transaction_transition_valid(
    UmiDistributionTransactionState current,
    UmiDistributionTransactionState next_state);
const char *umi_distribution_transaction_action_text(
    UmiDistributionTransactionAction action);
const char *umi_distribution_transaction_state_text(
    UmiDistributionTransactionState state);
UmiStatus umi_distribution_transaction_log_create(
    UmiDistributionTransactionLog **out_log);
void umi_distribution_transaction_log_destroy(UmiDistributionTransactionLog *log);
UmiStatus umi_distribution_transaction_log_upsert(
    UmiDistributionTransactionLog *log,
    const UmiDistributionTransaction *transaction);
UmiStatus umi_distribution_transaction_log_find(
    const UmiDistributionTransactionLog *log,
    const char *transaction_id,
    UmiDistributionTransaction *out_transaction);
UmiStatus umi_distribution_transaction_log_at(
    const UmiDistributionTransactionLog *log,
    size_t index,
    UmiDistributionTransaction *out_transaction);
size_t umi_distribution_transaction_log_count(
    const UmiDistributionTransactionLog *log);

#ifdef __cplusplus
}
#endif

#endif
