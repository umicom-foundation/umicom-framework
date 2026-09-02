/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/environment.c
 *
 * PURPOSE:
 *   Control whether an execution-capable action is permitted in a trading environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of environment. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/environment.h"
/*
 * Provide the trading environment allows live execution operation used by this module and
 * its client applications.
 */
int umi_trading_environment_allows_live_execution(UmiTradingEnvironment env,int armed){return env!=UMI_TRADING_LIVE||armed!=0;}
