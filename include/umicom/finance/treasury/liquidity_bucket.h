/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_bucket.h
 *
 * PURPOSE:
 *   Describe a liquidity time bucket and calculate its contractual gap.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_BUCKET_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_BUCKET_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLiquidityBucket {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t start_days;
    int32_t end_days;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryLiquidityBucket;
UmiStatus umi_treasury_liquidity_bucket_init(UmiTreasuryLiquidityBucket *value,
    const char *id,
    int32_t start_days,
    int32_t end_days,
    int64_t inflow_minor,
    int64_t outflow_minor);
bool umi_treasury_liquidity_bucket_valid(const UmiTreasuryLiquidityBucket *value);
int64_t umi_treasury_liquidity_bucket_gap_minor(const UmiTreasuryLiquidityBucket *value);
#ifdef __cplusplus
}
#endif
#endif
