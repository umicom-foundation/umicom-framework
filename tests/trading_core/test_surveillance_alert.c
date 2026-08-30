/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_surveillance_alert.c
 *
 * PURPOSE:
 *   Exercise capture explainable surveillance findings with stable participant and instrument identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/surveillance_alert.h"
int main(void) {

    UmiTradingSurveillanceAlert a;
    UmiFinancialId p,i;
    umi_trading_core_id_assign(&p,"p");
    umi_trading_core_id_assign(&i,"i");
    return umi_trading_surveillance_alert_init(&a,"a",&p,&i,UMI_TRADING_CORE_WARNING,"test",1)==UMI_STATUS_OK?0:1;
}
