/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_gap.c
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

#include "umicom/finance/enterprise/market_data_gap.h"

UmiStatus umi_enterprise_market_data_gap_detect(UmiEnterpriseMarketDataGap *gap,uint64_t expected,uint64_t received){ if(gap==NULL||expected==0U||received<expected)return UMI_STATUS_INVALID_ARGUMENT; gap->expected_sequence=expected; gap->received_sequence=received; gap->missing_count=received-expected; return UMI_STATUS_OK; }
