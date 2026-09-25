/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_confirmation.c
 *
 * PURPOSE:
 *   Verify FX Forward Confirmation Open TMS behaviour.
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

#include "umicom/open_tms/fx_forward/confirmation.h"

int main(void)
{
    UmiOpenTmsInput input;
    UmiOpenTmsSnapshot snapshot;

    umi_open_tms_input_init(&input);
    input.value = 80.0;
    input.secondaryValue = 70.0;
    input.referenceValue = 80.0;
    input.limitValue = 1000.0;
    input.amount = 100.0;
    input.notional = 1000000.0;
    input.spotRate = 1.25;
    input.domesticRate = 0.05;
    input.foreignRate = 0.03;
    input.yearFraction = 0.5;
    input.forwardRate = 1.2621359;
    input.discountFactor = 0.975609756;
    input.presentValue = 975.609756;
    input.accruedAmount = 25000.0;
    input.pnl = 250.0;
    input.grossExposure = 500.0;
    input.netExposure = 250.0;
    input.concentrationPercent = 10.0;
    input.liquidityInflows = 1200.0;
    input.liquidityOutflows = 1000.0;
    input.liquidityBuffer = 200.0;
    input.fundingAvailable = 2000.0;
    input.fundingRequired = 1000.0;
    input.settledCash = 1000.0;
    input.projectedCash = 1200.0;
    input.minimumCash = 500.0;
    input.matchedCount = 10U;
    input.totalCount = 10U;
    input.completedCount = 10U;
    input.failedCount = 0U;
    input.breakCount = 0U;
    input.breachedCount = 0U;
    input.ageMilliseconds = 100U;
    input.nowMilliseconds = 1000U;
    input.dueMilliseconds = 2000U;
    input.validIdentity = 1;
    input.active = 1;
    input.trusted = 1;
    input.approved = 1;
    input.marketDataReady = 1;
    input.curvesReady = 1;
    input.fixingsReady = 1;
    input.cashReady = 1;
    input.liquidityReady = 1;
    input.positionReady = 1;
    input.riskReady = 1;
    input.settlementReady = 1;
    input.accountingReady = 1;
    input.reconciliationReady = 1;
    input.auditReady = 1;
    input.transitionAllowed = 1;
    input.revision = 111U;

    assert(umi_open_tms_evaluate_fx_forward_confirmation(&input, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.id, "fx_forward.confirmation") == 0);
    assert(snapshot.revision == 111U);
    assert(snapshot.score >= 0.0 && snapshot.score <= 100.0);
    assert(snapshot.blocked == 0);
    assert(snapshot.ready == 1);
    return 0;
}
