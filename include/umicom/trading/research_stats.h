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
typedef struct UmiResearchStats { uint64_t observations; uint64_t qualifying_moves; double total_absolute_points; } UmiResearchStats;
void umi_research_stats_init(UmiResearchStats *stats);
void umi_research_stats_add(UmiResearchStats *stats, const UmiMovementEvent *event);
double umi_research_stats_mean_points(const UmiResearchStats *stats);
#ifdef __cplusplus
}
#endif
#endif
