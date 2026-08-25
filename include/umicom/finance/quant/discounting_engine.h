/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/discounting_engine.h
 *
 * PURPOSE:
 *   Price deterministic cashflows by exponential discounting.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_DISCOUNTING_ENGINE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_DISCOUNTING_ENGINE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Discount a single future cashflow. */
UmiStatus umi_quant_discounting_engine_cashflow(double amount, double zero_rate, int32_t days, double *out_pv);
/* Discount a bounded sequence of cashflows and rates. */
UmiStatus umi_quant_discounting_engine_portfolio(const double *amounts, const double *zero_rates, const int32_t *days, size_t count, double *out_pv);

#ifdef __cplusplus
}
#endif

#endif
