/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/fx_conversion.h
 *
 * PURPOSE:
 *   Convert UmiMoney through explicit ordered FX rates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FX_CONVERSION_H
#define UMICOM_FINANCE_CORE_FX_CONVERSION_H

#include "umicom/finance/core/exchange_rate.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Convert base money to quote money. */ UmiStatus umi_fx_conversion_base_to_quote(UmiMoney in,const UmiExchangeRate *r,uint8_t scale,UmiMoney *out);
/* Convert quote money to base money. */ UmiStatus umi_fx_conversion_quote_to_base(UmiMoney in,const UmiExchangeRate *r,uint8_t scale,UmiMoney *out);
#ifdef __cplusplus
}
#endif

#endif
