/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/research_window.h
 *
 * PURPOSE:
 *   Determine whether a research observation occurs inside the configured look-forward horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates research window behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_RESEARCH_WINDOW_H
#define INCLUDE_UMICOM_TRADING_RESEARCH_WINDOW_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the research window contains operation used by this module and its client
 * applications.
 */
int umi_research_window_contains(int64_t anchor_ms, int64_t event_ms, int64_t forward_ms);
#ifdef __cplusplus
}
#endif
#endif
