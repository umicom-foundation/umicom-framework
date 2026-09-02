/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/cash.c
 *
 * PURPOSE:
 *   Apply minor-unit cash movements to a same-currency balance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of cash. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/cash.h"
#include "umicom/finance/money.h"
/*
 * Perform cash through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_cash_apply(UmiMoney *b,const UmiMoney *m){UmiMoney out;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||m==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus s=umi_money_add(b,m,&out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)*b=out;return s;}
