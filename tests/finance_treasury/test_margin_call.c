/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_call.c
 *
 * PURPOSE:
 *   Exercise margin call validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call.h"
int main(void) {
    UmiTreasuryMarginCall v;
    if (umi_treasury_margin_call_init(&v, "mc", 1000, 900, UMI_TREASURY_MARGIN_AGREED) != UMI_STATUS_OK) return 1;
    if(umi_treasury_margin_call_unagreed_minor(&v)!=100)return 2;
    return 0;
}
