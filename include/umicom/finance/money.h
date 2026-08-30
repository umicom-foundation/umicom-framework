/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/money.h
 *
 * PURPOSE:
 *   Perform safe same-currency money addition and subtraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module keeps money logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_MONEY_H
#define INCLUDE_UMICOM_FINANCE_MONEY_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_money_add(const UmiMoney *left, const UmiMoney *right, UmiMoney *out);
UmiStatus umi_money_subtract(const UmiMoney *left, const UmiMoney *right, UmiMoney *out);
#ifdef __cplusplus
}
#endif
#endif
