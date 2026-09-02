/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/research_stats.h
 *
 * PURPOSE:
 *   Accumulate qualifying movement counts and mean absolute movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates research stats behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_RESEARCH_STATS_H
#define INCLUDE_UMICOM_TRADING_RESEARCH_STATS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the research stats data shared with callers of this public contract.
 */
typedef struct UmiResearchStats { uint64_t observations; uint64_t qualifying_moves; double total_absolute_points; } UmiResearchStats;
/**
 * Initialise research stats from caller-provided values so later operations receive a
 * known state.
 */
void umi_research_stats_init(UmiResearchStats *stats);
/**
 * Add research stats only after its inputs and available capacity have been checked.
 */
void umi_research_stats_add(UmiResearchStats *stats, const UmiMovementEvent *event);
/**
 * Provide the research stats mean points operation used by this module and its client
 * applications.
 */
double umi_research_stats_mean_points(const UmiResearchStats *stats);
#ifdef __cplusplus
}
#endif
#endif
