/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/quote.c
 *
 * PURPOSE:
 *   Calculate spread and midpoint from validated bid/ask quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of quote. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/quote.h"
int umi_quote_valid(const UmiQuote *q){return q!=NULL&&q->bid>0.0&&q->ask>=q->bid&&q->bid_size>=0.0&&q->ask_size>=0.0;}
double umi_quote_mid(const UmiQuote *q){return umi_quote_valid(q)?(q->bid+q->ask)*0.5:0.0;}
double umi_quote_spread(const UmiQuote *q){return umi_quote_valid(q)?q->ask-q->bid:0.0;}
