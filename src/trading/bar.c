/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/bar.c
 *
 * PURPOSE:
 *   Validate OHLCV bars and calculate range.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of bar. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/bar.h"
int umi_bar_valid(const UmiBar *b){return b!=NULL&&b->high>=b->low&&b->high>=b->open&&b->high>=b->close&&b->low<=b->open&&b->low<=b->close&&b->volume>=0.0&&b->end_time_ms>=b->start_time_ms;}
double umi_bar_range(const UmiBar *b){return umi_bar_valid(b)?b->high-b->low:0.0;}
double umi_bar_typical_price(const UmiBar *b){return umi_bar_valid(b)?(b->high+b->low+b->close)/3.0:0.0;}
