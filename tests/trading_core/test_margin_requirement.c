/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_margin_requirement.c
 *
 * PURPOSE:
 *   Exercise calculate integer notional margin requirements from bounded basis-point profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/margin_requirement.h"
int main(void) {

    UmiTradingMarginProfile p;
    UmiTradingMarginRequirement r;
    if(umi_trading_margin_profile_init(&p,5000U,3000U,500U)!=UMI_STATUS_OK)return 1;
    if(umi_trading_margin_requirement_compute(&p,100000,false,&r)!=UMI_STATUS_OK)return 2;
    return r.initial_minor==50000&&r.maintenance_minor==30000?0:3;
}
