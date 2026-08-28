/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/market_data_key.h
 *
 * PURPOSE:
 *   Define canonical market-data keys.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_MARKET_DATA_KEY_H
#define UMICOM_FINANCE_CORE_MARKET_DATA_KEY_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiMarketDataKey { UmiFinancialId key_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiMarketDataKey;
/* Initialize the typed financial record. */ UmiStatus umi_market_data_key_init(UmiMarketDataKey *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_market_data_key_is_valid(const UmiMarketDataKey *item);
#ifdef __cplusplus
}
#endif

#endif
