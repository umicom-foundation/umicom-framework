/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_rule.c
 *
 * PURPOSE:
 *   Exercise posting rule validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_rule.h"
int main(void) {
    UmiAccountingPostingRule v;
    if(umi_accounting_posting_rule_init(&v, "rule-1", "CASH_RECEIPT", "1000", "4000", true)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_posting_rule_valid(&v)) return 2;
    return 0;
}
