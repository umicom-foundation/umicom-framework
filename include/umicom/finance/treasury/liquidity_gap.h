/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_gap.h
 *
 * PURPOSE:
 *   Represent a currency liquidity mismatch for a defined horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury liquidity gap data shared with callers of this public contract.
 */
typedef struct UmiTreasuryLiquidityGap {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t horizon_days;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryLiquidityGap;
/**
 * Initialise treasury liquidity gap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_gap_init(UmiTreasuryLiquidityGap *value,
    const char *id,
    int32_t horizon_days,
    int64_t inflow_minor,
    int64_t outflow_minor);
/**
 * Check that treasury liquidity gap satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_liquidity_gap_valid(const UmiTreasuryLiquidityGap *value);
/**
 * Provide the treasury liquidity gap net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_liquidity_gap_net_minor(const UmiTreasuryLiquidityGap *value);
#ifdef __cplusplus
}
#endif
#endif
