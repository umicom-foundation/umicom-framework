/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_account_hierarchy.c
 *
 * PURPOSE:
 *   Exercise account hierarchy validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/account_hierarchy.h"
int main(void) {
    UmiAccountingAccountHierarchy v;
    if(umi_accounting_account_hierarchy_init(&v, "edge-1", "1000", "1010")!=UMI_STATUS_OK) return 1;
    if(umi_accounting_account_hierarchy_self_reference(&v)) return 2;
    return 0;
}
