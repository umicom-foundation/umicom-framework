/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/currency.h
 *
 * PURPOSE:
 *   Validate three-letter uppercase currency codes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module keeps currency logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_CURRENCY_H
#define INCLUDE_UMICOM_FINANCE_CURRENCY_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_currency_valid(const UmiCurrency *currency);
#ifdef __cplusplus
}
#endif
#endif
