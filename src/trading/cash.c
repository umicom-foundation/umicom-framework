/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/cash.c
 *
 * PURPOSE:
 *   Apply minor-unit cash movements to a same-currency balance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of cash. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/cash.h"
#include "umicom/finance/money.h"
UmiStatus umi_cash_apply(UmiMoney *b,const UmiMoney *m){UmiMoney out;if(b==NULL||m==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus s=umi_money_add(b,m,&out);if(s==UMI_STATUS_OK)*b=out;return s;}
