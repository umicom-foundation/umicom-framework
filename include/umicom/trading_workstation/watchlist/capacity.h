/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/watchlist/capacity.h
 *
 * PURPOSE:
 *   Evaluate Watchlist Capacity for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_WATCHLIST_CAPACITY_H
#define UMICOM_TRADING_WORKSTATION_WATCHLIST_CAPACITY_H

#include <stddef.h>
#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_watchlist_capacity(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus umi_trading_professional_evaluate_watchlist_capacity_batch(const UmiTradingProfessionalInput *inputs,
    size_t count, UmiTradingProfessionalSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
