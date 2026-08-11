/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/account.h
 *
 * PURPOSE:
 *   Validate financial account identity, owner and settlement currency.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module keeps account logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_ACCOUNT_H
#define INCLUDE_UMICOM_FINANCE_ACCOUNT_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_financial_account_valid(const UmiFinancialAccount *account);
#ifdef __cplusplus
}
#endif
#endif
