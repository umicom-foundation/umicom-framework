/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_asset.h
 *
 * PURPOSE:
 *   Represent an eligible liquidity asset and calculate post-haircut value.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_ASSET_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_ASSET_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityAsset { char asset_id[UMI_PRU_ID_CAPACITY]; UmiHqlaLevel level; double market_value; double haircut; double eligible_value; UmiCurrency currency; } UmiPrudentialLiquidityAsset;
/* Initialise a liquidity asset and derive post-haircut eligible value. */
UmiStatus umi_pru_liquidity_asset_init(UmiPrudentialLiquidityAsset *asset, const char *asset_id, UmiHqlaLevel level, double market_value, double haircut, UmiCurrency currency);

#ifdef __cplusplus
}
#endif

#endif
