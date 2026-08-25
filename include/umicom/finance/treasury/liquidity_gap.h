/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_gap.h
 *
 * PURPOSE:
 *   Represent a currency liquidity mismatch for a defined horizon.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLiquidityGap {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t horizon_days;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryLiquidityGap;
UmiStatus umi_treasury_liquidity_gap_init(UmiTreasuryLiquidityGap *value,
    const char *id,
    int32_t horizon_days,
    int64_t inflow_minor,
    int64_t outflow_minor);
bool umi_treasury_liquidity_gap_valid(const UmiTreasuryLiquidityGap *value);
int64_t umi_treasury_liquidity_gap_net_minor(const UmiTreasuryLiquidityGap *value);
#ifdef __cplusplus
}
#endif
#endif
