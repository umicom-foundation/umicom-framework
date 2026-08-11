/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/health.c
 *
 * PURPOSE:
 *   Combine market-data, broker and risk readiness into one trading health decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of health. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/health.h"
int umi_trading_health_ready(int market,int broker,int risk,UmiTradingEnvironment env){if(!market||!risk)return 0;if(env==UMI_TRADING_SIMULATION)return 1;return broker!=0;}
