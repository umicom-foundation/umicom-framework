/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/price_band.c
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
#include "umicom/trading/core/price_band.h"

#include <stdint.h>
/* Initialise a positive reference price and basis-point bands. */
UmiStatus umi_trading_price_band_init(UmiTradingPriceBand *band,UmiTradingPriceTicks reference_price,uint32_t lower_bps,uint32_t upper_bps){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(band==NULL||reference_price<=0||lower_bps>10000U||upper_bps>10000U)return UMI_STATUS_INVALID_ARGUMENT;band->reference_price=reference_price;band->lower_bps=lower_bps;band->upper_bps=upper_bps;return UMI_STATUS_OK;}
/* Compare a positive difference with a basis-point allowance using quotient/remainder decomposition to avoid reference*bps overflow. */
static bool within_bps(UmiTradingPriceTicks reference,UmiTradingPriceTicks difference,uint32_t bps){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(difference<0)return false;int64_t q=reference/10000;int64_t r=reference%10000;uint64_t allowance=(uint64_t)q*(uint64_t)bps+((uint64_t)r*(uint64_t)bps)/10000U;return (uint64_t)difference<=allowance;}
/* Determine whether a price is inside the configured basis-point band. */
bool umi_trading_price_band_contains(const UmiTradingPriceBand *band,UmiTradingPriceTicks price){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(band==NULL||price<0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(price>=band->reference_price)return within_bps(band->reference_price,price-band->reference_price,band->upper_bps);return within_bps(band->reference_price,band->reference_price-price,band->lower_bps);}
