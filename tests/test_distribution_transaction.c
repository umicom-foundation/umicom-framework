/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_transaction.c
 *
 * PURPOSE:
 *   Verify explicit update state transitions, recoverability and transaction
 *   history.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>

#include "distribution_fixture.h"

int main(void)
{
    UmiDistributionTransaction transaction;
    UmiDistributionTransaction stored;
    UmiDistributionTransactionLog *log = NULL;
    assert(umi_distribution_transaction_plan(
        &transaction, "txn-1", UMI_DISTRIBUTION_UPDATE, "org.umicom.studio",
        "studio-0.17.0", (UmiVersion){0U, 16U, 0U},
        (UmiVersion){0U, 17U, 0U}, "C:/Program Files/Umicom Studio", 100U) ==
        UMI_STATUS_OK);
    assert(transaction.step_count >= 7U);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_APPLYING,
        "too early", 110U) == UMI_STATUS_INVALID_STATE);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_STAGING,
        "staging", 120U) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_VERIFYING,
        "verifying", 130U) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK,
        "verification recovery", 140U) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK,
        "restored", 150U) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_log_create(&log) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_log_upsert(log, &transaction) ==
           UMI_STATUS_OK);
    assert(umi_distribution_transaction_log_find(log, "txn-1", &stored) ==
           UMI_STATUS_OK);
    assert(stored.state == UMI_DISTRIBUTION_TRANSACTION_ROLLED_BACK);
    assert(umi_distribution_transaction_plan(
        &transaction, "txn-install", UMI_DISTRIBUTION_INSTALL,
        "org.umicom.c-tools", "c-tools-1.1.0", (UmiVersion){0U, 0U, 0U},
        (UmiVersion){1U, 1U, 0U}, ".umicom/products", 200U) == UMI_STATUS_OK);
    assert(transaction.reversible);
    assert(umi_distribution_transaction_plan(
        &transaction, "txn-uninstall", UMI_DISTRIBUTION_UNINSTALL,
        "org.umicom.c-tools", "c-tools-1.1.0", (UmiVersion){1U, 1U, 0U},
        (UmiVersion){0U, 0U, 0U}, ".umicom/products", 300U) == UMI_STATUS_OK);
    assert(!transaction.reversible);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_FAILED,
        "uninstall failed", 310U) == UMI_STATUS_OK);
    assert(umi_distribution_transaction_transition(
        &transaction, UMI_DISTRIBUTION_TRANSACTION_ROLLING_BACK,
        "rollback is forbidden", 320U) == UMI_STATUS_PERMISSION_DENIED);
    umi_distribution_transaction_log_destroy(log);
    return 0;
}
