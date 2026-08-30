/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_quality.h
 *
 * PURPOSE:
 *   Score completeness and anomaly evidence for a market-data cut.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_QUALITY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_QUALITY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseMarketDataQuality { size_t expected; size_t observed; size_t invalid; size_t outliers; } UmiEnterpriseMarketDataQuality;
/* Initialise quality evidence counters for an expected topic population. */
UmiStatus umi_enterprise_market_data_quality_init(UmiEnterpriseMarketDataQuality *quality, size_t expected);
/* Calculate a bounded quality score in [0,1]. */
double umi_enterprise_market_data_quality_score(const UmiEnterpriseMarketDataQuality *quality);

#ifdef __cplusplus
}
#endif

#endif
