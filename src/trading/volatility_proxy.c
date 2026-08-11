/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/volatility_proxy.c
 *
 * PURPOSE:
 *   Calculate a simple bar-range volatility proxy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of volatility proxy. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/volatility_proxy.h"
#include "umicom/trading/bar.h"
double umi_bar_volatility_proxy(const UmiBar *b){if(!umi_bar_valid(b)||b->close<=0.0)return 0.0;return umi_bar_range(b)/b->close;}
