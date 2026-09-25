/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_account_available_funds.c
 *
 * PURPOSE:
 *   Verify Available Funds shared-financial-platform behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/financial_platform/account/available_funds.h"

int main(void)
{
    UmiFinancialPlatformInput input;
    UmiFinancialPlatformSnapshot snapshot;

    umi_financial_platform_input_init(&input);
    input.value = 100.0;
    input.secondaryValue = 50.0;
    input.referenceValue = 100.0;
    input.limitValue = 1000.0;
    input.tolerance = 1.0;
    input.amount = 100.0;
    input.convertedAmount = 125.0;
    input.rate = 1.25;
    input.debitTotal = 1000.0;
    input.creditTotal = 1000.0;
    input.grossExposure = 500.0;
    input.netExposure = 250.0;
    input.pnl = 25.0;
    input.concentrationPercent = 10.0;
    input.suspenseAmount = 0.0;
    input.totalAmount = 1000.0;
    input.matchedCount = 10U;
    input.totalCount = 10U;
    input.mismatchCount = 0U;
    input.pendingCount = 0U;
    input.approvedCount = 10U;
    input.rejectedCount = 0U;
    input.completedCount = 10U;
    input.exceptionCount = 0U;
    input.auditCount = 10U;
    input.persistedCount = 10U;
    input.gapCount = 0U;
    input.ageMilliseconds = 100U;
    input.nowMilliseconds = 1000U;
    input.dueMilliseconds = 2000U;
    input.validIdentity = 1;
    input.active = 1;
    input.trusted = 1;
    input.approved = 1;
    input.currencyReady = 1;
    input.partyReady = 1;
    input.accountReady = 1;
    input.instrumentReady = 1;
    input.tradeReady = 1;
    input.positionReady = 1;
    input.paymentReady = 1;
    input.ledgerReady = 1;
    input.settlementReady = 1;
    input.reconciliationReady = 1;
    input.workflowReady = 1;
    input.auditReady = 1;
    input.transitionAllowed = 1;
    input.rollbackAvailable = 1;
    input.revision = 101U;

    assert(umi_financial_platform_evaluate_account_available_funds(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "account.available-funds") == 0);
    assert(snapshot.revision == 101U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1 || snapshot.attention == 0 || snapshot.value >= 0.0);
    return 0;
}
