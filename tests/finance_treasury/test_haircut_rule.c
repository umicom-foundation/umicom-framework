/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_haircut_rule.c
 *
 * PURPOSE:
 *   Exercise haircut rule validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/haircut_rule.h"
int main(void) {
    UmiTreasuryHaircutRule v;
    if (umi_treasury_haircut_rule_init(&v, "hc", 250U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_haircut_rule_remaining_bps(&v)!=9750U)return 2;
    return 0;
}
