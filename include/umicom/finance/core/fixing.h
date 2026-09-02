/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/fixing.h
 *
 * PURPOSE:
 *   Represent dated observed financial fixings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FIXING_H
#define UMICOM_FINANCE_CORE_FIXING_H

#include "umicom/finance/core/market_data_key.h"
#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the fixing data shared with callers of this public contract.
 */
typedef struct UmiFixing { UmiMarketDataKey key; UmiFinancialDate date; double value; int64_t timestamp; } UmiFixing;
/* Initialize fixing. */ UmiStatus umi_fixing_init(UmiFixing *f,const UmiMarketDataKey *k,UmiFinancialDate d,double value,int64_t ts);
/* Validate fixing. */ bool umi_fixing_is_valid(const UmiFixing *f);
#ifdef __cplusplus
}
#endif

#endif
