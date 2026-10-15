/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest/trade_count.h
 *
 * PURPOSE:
 *   Evaluate Backtest Trade Count as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_TRADE_COUNT_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_TRADE_COUNT_H

#include <stddef.h>
#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_backtest_trade_count(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus umi_strategy_research_evaluate_backtest_trade_count_batch(const UmiStrategyResearchInput *inputs,
    size_t count, UmiStrategyResearchSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
