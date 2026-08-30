/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_finance_foundation.c
 *
 * PURPOSE:
 *   Validate finance foundation behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/finance/finance.h"
int main(void){
    UmiFinancialId id={0};(void)snprintf(id.value,sizeof(id.value),"%s","A-1");assert(umi_financial_id_valid(&id));
    UmiCurrency usd={{'U','S','D','\0'}};assert(umi_currency_valid(&usd));
    UmiMoney a={1000,2,{{'U','S','D','\0'}}},b={250,2,{{'U','S','D','\0'}}},out={0};
    assert(umi_money_add(&a,&b,&out)==UMI_STATUS_OK);assert(out.minor_units==1250);
    UmiDecimal d={123,2},r={0};assert(umi_decimal_rescale(d,4U,&r)==UMI_STATUS_OK);assert(r.coefficient==12300);
    return 0;
}
