/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/types.h
 *
 * PURPOSE:
 *   Define bounded Open TMS evidence for FX-forward lifecycle, cash/liquidity,
 *   pricing, positions, risk, settlement, accounting and reconciliation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_TYPES_H
#define UMICOM_OPEN_TMS_TYPES_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_ID_CAPACITY 96U
#define UMI_OPEN_TMS_LABEL_CAPACITY 128U

typedef struct UmiOpenTmsInput {
    const char *label;

    double value;
    double secondaryValue;
    double referenceValue;
    double limitValue;
    double amount;
    double notional;
    double spotRate;
    double domesticRate;
    double foreignRate;
    double yearFraction;
    double forwardRate;
    double discountFactor;
    double presentValue;
    double accruedAmount;
    double pnl;
    double grossExposure;
    double netExposure;
    double concentrationPercent;
    double liquidityInflows;
    double liquidityOutflows;
    double liquidityBuffer;
    double fundingAvailable;
    double fundingRequired;
    double settledCash;
    double projectedCash;
    double minimumCash;

    uint64_t matchedCount;
    uint64_t totalCount;
    uint64_t completedCount;
    uint64_t failedCount;
    uint64_t breakCount;
    uint64_t breachedCount;
    uint64_t ageMilliseconds;
    uint64_t nowMilliseconds;
    uint64_t dueMilliseconds;

    int validIdentity;
    int active;
    int trusted;
    int approved;
    int marketDataReady;
    int curvesReady;
    int fixingsReady;
    int cashReady;
    int liquidityReady;
    int positionReady;
    int riskReady;
    int settlementReady;
    int accountingReady;
    int reconciliationReady;
    int auditReady;
    int transitionAllowed;

    uint64_t revision;
} UmiOpenTmsInput;

typedef struct UmiOpenTmsSnapshot {
    char id[UMI_OPEN_TMS_ID_CAPACITY];
    char label[UMI_OPEN_TMS_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    double pnl;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    uint64_t revision;
} UmiOpenTmsSnapshot;

typedef UmiStatus (*UmiOpenTmsEvaluator)(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

void umi_open_tms_input_init(UmiOpenTmsInput *input);
void umi_open_tms_snapshot_init(UmiOpenTmsSnapshot *snapshot);
UmiStatus umi_open_tms_snapshot_validate(const UmiOpenTmsSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
