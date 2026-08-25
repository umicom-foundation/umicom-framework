/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_audit.c
 *
 * PURPOSE:
 *   Exercise treasury audit validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_audit.h"
int main(void) {
    UmiTreasuryTreasuryAudit v;
    if (umi_treasury_treasury_audit_init(&v, "audit", "operator", 7U) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_treasury_audit_sequenced(&v))return 2;
    return 0;
}
