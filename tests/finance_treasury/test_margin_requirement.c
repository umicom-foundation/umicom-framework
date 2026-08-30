/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_requirement.c
 *
 * PURPOSE:
 *   Exercise margin requirement validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_requirement.h"
int main(void) {
    UmiTreasuryMarginRequirement v;
    if (umi_treasury_margin_requirement_init(&v, "mr", 1000, 200, 50) != UMI_STATUS_OK) return 1;
    if(umi_treasury_margin_requirement_required_minor(&v)!=850)return 2;
    return 0;
}
