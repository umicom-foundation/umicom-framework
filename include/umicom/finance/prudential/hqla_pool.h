/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/hqla_pool.h
 *
 * PURPOSE:
 *   Aggregate HQLA assets into an available liquidity stock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_HQLA_POOL_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_HQLA_POOL_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/liquidity_asset.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential hqla pool data shared with callers of this public contract.
 */
typedef struct UmiPrudentialHqlaPool { UmiPrudentialLiquidityAsset assets[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialHqlaPool;
/* Add one eligible liquidity asset to the bounded HQLA pool. */
UmiStatus umi_pru_hqla_pool_add(UmiPrudentialHqlaPool *pool, const UmiPrudentialLiquidityAsset *asset);
/* Sum post-haircut eligible HQLA values across the pool. */
double umi_pru_hqla_pool_eligible_total(const UmiPrudentialHqlaPool *pool);

#ifdef __cplusplus
}
#endif

#endif
