/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_gap.h
 *
 * PURPOSE:
 *   Represent deterministic feed sequence gaps for recovery workflows.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_GAP_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_GAP_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseMarketDataGap { uint64_t expected_sequence; uint64_t received_sequence; uint64_t missing_count; } UmiEnterpriseMarketDataGap;
/* Derive missing sequence evidence when a feed jumps forward. */
UmiStatus umi_enterprise_market_data_gap_detect(UmiEnterpriseMarketDataGap *gap, uint64_t expected_sequence, uint64_t received_sequence);

#ifdef __cplusplus
}
#endif

#endif
