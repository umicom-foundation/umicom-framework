/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/price_band.h
 *
 * PURPOSE:
 *   Define price protection bands around a reference price without overflow-prone multiplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_PRICE_BAND_H
#define UMICOM_TRADING_CORE_PRICE_BAND_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingPriceBand { UmiTradingPriceTicks reference_price; uint32_t lower_bps; uint32_t upper_bps; } UmiTradingPriceBand;
/* Initialise a positive reference price and basis-point bands. */
UmiStatus umi_trading_price_band_init(UmiTradingPriceBand *band,UmiTradingPriceTicks reference_price,uint32_t lower_bps,uint32_t upper_bps);
/* Determine whether a price is inside the configured basis-point band. */
bool umi_trading_price_band_contains(const UmiTradingPriceBand *band,UmiTradingPriceTicks price);
#ifdef __cplusplus
}
#endif
#endif
