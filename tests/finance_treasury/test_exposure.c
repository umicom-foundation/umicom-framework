/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_exposure.c
 *
 * PURPOSE:
 *   Exercise exposure validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/exposure.h"
int main(void) {
    UmiTreasuryExposure v;
    if (umi_treasury_exposure_init(&v, "exp", 1000, -600) != UMI_STATUS_OK) return 1;
    if(umi_treasury_exposure_net_absolute_minor(&v)!=600)return 2;
    return 0;
}
