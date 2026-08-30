/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/circuit_breaker.c
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
#include "umicom/trading/core/circuit_breaker.h"
#include <limits.h>

UmiStatus umi_trading_circuit_breaker_init(UmiTradingCircuitBreaker *breaker,uint32_t threshold_bps,uint32_t halt_seconds){if(breaker==NULL||threshold_bps==0U||halt_seconds==0U)return UMI_STATUS_INVALID_ARGUMENT;breaker->threshold_bps=threshold_bps;breaker->halt_seconds=halt_seconds;breaker->halted=false;breaker->halt_until_ms=0;return UMI_STATUS_OK;}
bool umi_trading_circuit_breaker_evaluate(UmiTradingCircuitBreaker *breaker,uint32_t absolute_move_bps,int64_t now_ms){if(breaker==NULL||now_ms<0)return false;if(absolute_move_bps>=breaker->threshold_bps){int64_t duration=(int64_t)breaker->halt_seconds*1000;if(now_ms>INT64_MAX-duration)return false;breaker->halted=true;breaker->halt_until_ms=now_ms+duration;return true;}return umi_trading_circuit_breaker_halted(breaker,now_ms);}
bool umi_trading_circuit_breaker_halted(UmiTradingCircuitBreaker *breaker,int64_t now_ms){if(breaker==NULL||now_ms<0)return false;if(breaker->halted&&now_ms>=breaker->halt_until_ms)breaker->halted=false;return breaker->halted;}
