/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_service_catalogue.c
 *
 * PURPOSE:
 *   Verify all Open TMS policy capabilities are discoverable and executable.
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
#include "umicom/open_tms/service.h"
int main(void)
{
    UmiOpenTmsInput input; UmiOpenTmsSnapshot snapshot;
    umi_open_tms_input_init(&input);
    input.value=80.0;input.secondaryValue=70.0;input.referenceValue=80.0;input.limitValue=1000.0;
    input.amount=100.0;input.notional=1000000.0;input.spotRate=1.25;input.domesticRate=.05;input.foreignRate=.03;
    input.yearFraction=.5;input.forwardRate=1.2621359;input.discountFactor=.9756;input.presentValue=975.6;input.accruedAmount=25000.0;
    input.pnl=250.0;input.grossExposure=500.0;input.netExposure=250.0;input.concentrationPercent=10.0;
    input.liquidityInflows=1200.0;input.liquidityOutflows=1000.0;input.liquidityBuffer=200.0;input.fundingAvailable=2000.0;input.fundingRequired=1000.0;
    input.settledCash=1000.0;input.projectedCash=1200.0;input.minimumCash=500.0;
    input.matchedCount=10U;input.totalCount=10U;input.completedCount=10U;input.nowMilliseconds=1000U;input.dueMilliseconds=2000U;
    input.validIdentity=1;input.active=1;input.trusted=1;input.approved=1;input.marketDataReady=1;input.curvesReady=1;input.fixingsReady=1;
    input.cashReady=1;input.liquidityReady=1;input.positionReady=1;input.riskReady=1;input.settlementReady=1;input.accountingReady=1;
    input.reconciliationReady=1;input.auditReady=1;input.transitionAllowed=1;input.revision=9U;
    assert(umi_open_tms_service_count()==80U);
    for(size_t i=0U;i<umi_open_tms_service_count();++i){const char *id=NULL;
        assert(umi_open_tms_service_id_at(i,&id)==UMI_STATUS_OK);
        assert(umi_open_tms_service_evaluate(id,&input,&snapshot)==UMI_STATUS_OK);
        assert(strcmp(snapshot.id,id)==0);}
    return 0;
}
