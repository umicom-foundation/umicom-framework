/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/buying_power_model.h
 *
 * PURPOSE:
 *   Compute reusable brokerage buying power from cash, equity and margin requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BUYING_POWER_MODEL_H
#define UMICOM_TRADING_CORE_BUYING_POWER_MODEL_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Compute non-negative buying power after reserving initial margin. */
UmiStatus umi_trading_buying_power_model_compute(int64_t cash_minor,int64_t equity_minor,int64_t reserved_margin_minor,int64_t *out_buying_power_minor);
#ifdef __cplusplus
}
#endif
#endif
