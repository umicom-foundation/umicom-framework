/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_financial_control.c
 *
 * PURPOSE:
 *   Exercise financial control validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/financial_control.h"
int main(void) {
    UmiAccountingFinancialControl v;
    if(umi_accounting_financial_control_init(&v, "control-1", 0U, 0U, 1U)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_financial_control_healthy(&v)) return 2;
    return 0;
}
