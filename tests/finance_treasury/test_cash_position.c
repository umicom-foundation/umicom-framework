/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_position.c
 *
 * PURPOSE:
 *   Exercise cash position validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_position.h"
int main(void) {
    UmiTreasuryCashPosition v;
    if (umi_treasury_cash_position_init(&v, "pos-1", "USD", 500, 425) != UMI_STATUS_OK) return 1;
    if (umi_treasury_cash_position_delta_minor(&v) != -75) return 2;
    return 0;
}
