/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/volatility_proxy.c
 *
 * PURPOSE:
 *   Calculate a simple bar-range volatility proxy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of volatility proxy. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/volatility_proxy.h"
#include "umicom/trading/bar.h"
/*
 * Provide the bar volatility proxy operation used by this module and its client
 * applications.
 */
double umi_bar_volatility_proxy(const UmiBar *b){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_bar_valid(b)||b->close<=0.0)return 0.0;return umi_bar_range(b)/b->close;}
