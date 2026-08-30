/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/circuit_breaker.h
 *
 * PURPOSE:
 *   Apply venue-level price-move thresholds and timed halt state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_CIRCUIT_BREAKER_H
#define UMICOM_TRADING_CORE_CIRCUIT_BREAKER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingCircuitBreaker { uint32_t threshold_bps; uint32_t halt_seconds; bool halted; int64_t halt_until_ms; } UmiTradingCircuitBreaker;
UmiStatus umi_trading_circuit_breaker_init(UmiTradingCircuitBreaker *breaker,uint32_t threshold_bps,uint32_t halt_seconds);
/* Evaluate a move in basis points and enter a timed halt when the threshold is reached. */
bool umi_trading_circuit_breaker_evaluate(UmiTradingCircuitBreaker *breaker,uint32_t absolute_move_bps,int64_t now_ms);
/* Refresh the timed halt and return current halted state. */
bool umi_trading_circuit_breaker_halted(UmiTradingCircuitBreaker *breaker,int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
