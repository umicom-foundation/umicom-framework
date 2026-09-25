/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/types.h
 *
 * PURPOSE:
 *   Define cross-domain evidence for the shared financial platform control
 *   plane spanning reference data, accounts, parties, instruments, trades,
 *   positions, payments, ledger, settlement, reconciliation, workflow and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_TYPES_H
#define UMICOM_FINANCIAL_PLATFORM_TYPES_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_PLATFORM_ID_CAPACITY 96U
#define UMI_FINANCIAL_PLATFORM_LABEL_CAPACITY 128U

typedef struct UmiFinancialPlatformInput {
    const char *label;

    double value;
    double secondaryValue;
    double referenceValue;
    double limitValue;
    double tolerance;
    double amount;
    double convertedAmount;
    double rate;
    double debitTotal;
    double creditTotal;
    double grossExposure;
    double netExposure;
    double pnl;
    double concentrationPercent;
    double suspenseAmount;
    double totalAmount;

    uint64_t matchedCount;
    uint64_t totalCount;
    uint64_t mismatchCount;
    uint64_t pendingCount;
    uint64_t approvedCount;
    uint64_t rejectedCount;
    uint64_t completedCount;
    uint64_t exceptionCount;
    uint64_t auditCount;
    uint64_t persistedCount;
    uint64_t gapCount;
    uint64_t ageMilliseconds;
    uint64_t nowMilliseconds;
    uint64_t dueMilliseconds;

    int validIdentity;
    int active;
    int trusted;
    int approved;
    int currencyReady;
    int partyReady;
    int accountReady;
    int instrumentReady;
    int tradeReady;
    int positionReady;
    int paymentReady;
    int ledgerReady;
    int settlementReady;
    int reconciliationReady;
    int workflowReady;
    int auditReady;
    int transitionAllowed;
    int rollbackAvailable;

    uint64_t revision;
} UmiFinancialPlatformInput;

typedef struct UmiFinancialPlatformSnapshot {
    char id[UMI_FINANCIAL_PLATFORM_ID_CAPACITY];
    char label[UMI_FINANCIAL_PLATFORM_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    double delta;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    uint64_t revision;
} UmiFinancialPlatformSnapshot;

typedef UmiStatus (*UmiFinancialPlatformEvaluator)(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

void umi_financial_platform_input_init(UmiFinancialPlatformInput *input);
void umi_financial_platform_snapshot_init(UmiFinancialPlatformSnapshot *snapshot);
UmiStatus umi_financial_platform_snapshot_validate(
    const UmiFinancialPlatformSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
