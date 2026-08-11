/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_cash_buying_power.c
 *
 * PURPOSE:
 *   Validate cash buying power behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "umicom/trading/trading.h"
int main(void){UmiMoney balance={10000,2,{{'U','S','D','\0'}}};UmiMoney move={-2500,2,{{'U','S','D','\0'}}};assert(umi_cash_apply(&balance,&move)==UMI_STATUS_OK);assert(balance.minor_units==7500);assert(umi_buying_power(1000,4)==4000);assert(umi_execution_fee(2,1.5,1)==3);return 0;}
