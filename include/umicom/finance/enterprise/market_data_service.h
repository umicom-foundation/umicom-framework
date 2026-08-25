/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_service.h
 *
 * PURPOSE:
 *   Aggregate feed cache and freshness evidence into a reusable market-data service.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SERVICE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/market_data_cache.h"
#include "umicom/finance/enterprise/market_data_freshness.h"

typedef struct UmiEnterpriseMarketDataService { UmiEnterpriseMarketDataCache cache; UmiEnterpriseMarketDataFreshnessPolicy freshness; uint64_t accepted_updates; uint64_t rejected_updates; } UmiEnterpriseMarketDataService;
/* Initialise the Framework-owned market-data service with a freshness policy. */
UmiStatus umi_enterprise_market_data_service_init(UmiEnterpriseMarketDataService *service,int64_t max_age_ms);
/* Accept an ordered update into the latest-value cache and evidence counters. */
UmiStatus umi_enterprise_market_data_service_publish(UmiEnterpriseMarketDataService *service,const UmiEnterpriseMarketDataUpdate *update);

#ifdef __cplusplus
}
#endif

#endif
