/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_event.c
 *
 * PURPOSE:
 *   Exercise treasury event validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_event.h"
int main(void) {
    UmiTreasuryTreasuryEvent v;
    if (umi_treasury_treasury_event_init(&v, "event", 1U, 1000) != UMI_STATUS_OK) return 1;
    if(umi_treasury_treasury_event_event_sequence(&v)!=1U)return 2;
    return 0;
}
