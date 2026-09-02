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

/*
 * Initialise trading circuit breaker from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_circuit_breaker_init(UmiTradingCircuitBreaker *breaker,uint32_t threshold_bps,uint32_t halt_seconds){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(breaker==NULL||threshold_bps==0U||halt_seconds==0U)return UMI_STATUS_INVALID_ARGUMENT;breaker->threshold_bps=threshold_bps;breaker->halt_seconds=halt_seconds;breaker->halted=false;breaker->halt_until_ms=0;return UMI_STATUS_OK;}
/*
 * Provide the trading circuit breaker evaluate operation used by this module and its
 * client applications.
 */
bool umi_trading_circuit_breaker_evaluate(UmiTradingCircuitBreaker *breaker,uint32_t absolute_move_bps,int64_t now_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(breaker==NULL||now_ms<0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(absolute_move_bps>=breaker->threshold_bps){int64_t duration=(int64_t)breaker->halt_seconds*1000;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(now_ms>INT64_MAX-duration)return false;breaker->halted=true;breaker->halt_until_ms=now_ms+duration;return true;}return umi_trading_circuit_breaker_halted(breaker,now_ms);}
/*
 * Provide the trading circuit breaker halted operation used by this module and its client
 * applications.
 */
bool umi_trading_circuit_breaker_halted(UmiTradingCircuitBreaker *breaker,int64_t now_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(breaker==NULL||now_ms<0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(breaker->halted&&now_ms>=breaker->halt_until_ms)breaker->halted=false;return breaker->halted;}
