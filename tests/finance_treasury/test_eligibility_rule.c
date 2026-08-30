/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_eligibility_rule.c
 *
 * PURPOSE:
 *   Exercise eligibility rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/eligibility_rule.h"
int main(void) {
    UmiTreasuryEligibilityRule v;
    if (umi_treasury_eligibility_rule_init(&v, "elig", 100, 365U) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_eligibility_rule_usable(&v))return 2;
    return 0;
}
