/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_service.h
 *
 * PURPOSE:
 *   Aggregate market-data readiness and valuation-grid capacity into service health.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_SERVICE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/market_data_service.h"
#include "umicom/finance/enterprise/valuation_grid.h"
typedef struct UmiEnterpriseValuationService { UmiEnterpriseMarketDataService market_data; UmiEnterpriseValuationGrid grid; uint64_t completed_jobs; uint64_t failed_jobs; } UmiEnterpriseValuationService;
/* Initialise the valuation service and its internal market-data/grid control planes. */
UmiStatus umi_enterprise_valuation_service_init(UmiEnterpriseValuationService *service,int64_t freshness_ms);
/* Return one when at least one worker and one accepted market-data update are available. */
int umi_enterprise_valuation_service_ready(const UmiEnterpriseValuationService *service);

#ifdef __cplusplus
}
#endif

#endif
