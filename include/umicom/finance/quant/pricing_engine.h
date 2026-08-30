/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/pricing_engine.h
 *
 * PURPOSE:
 *   Route reusable product families to deterministic pricing kernels.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_PRICING_ENGINE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_PRICING_ENGINE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a compact generic request using class-specific linear kernels. */
UmiStatus umi_quant_pricing_engine_value(UmiQuantProductClass product_class, double quantity, double market_value, double strike_or_reference, double discount_factor, double *out_pv);

#ifdef __cplusplus
}
#endif

#endif
