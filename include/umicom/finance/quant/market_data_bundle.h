/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/market_data_bundle.h
 *
 * PURPOSE:
 *   Collect bounded named quantitative market inputs for one valuation.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MARKET_DATA_BUNDLE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MARKET_DATA_BUNDLE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant market data bundle data shared with callers of this public contract.
 */
typedef struct UmiQuantMarketDataBundle { UmiQuantNamedValue values[128]; size_t count; } UmiQuantMarketDataBundle;
/* Initialise an empty valuation market-data bundle. */
void umi_quant_market_data_bundle_init(UmiQuantMarketDataBundle *bundle);
/* Add one unique named market datum. */
UmiStatus umi_quant_market_data_bundle_add(UmiQuantMarketDataBundle *bundle, const char *name, double value);
/* Find a market datum by exact identifier. */
UmiStatus umi_quant_market_data_bundle_find(const UmiQuantMarketDataBundle *bundle, const char *name, double *out_value);

#ifdef __cplusplus
}
#endif

#endif
