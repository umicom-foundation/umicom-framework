/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_data_snapshot.c
 *
 * PURPOSE:
 *   Determine whether quote and bar evidence are time-aligned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of market data snapshot. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_data_snapshot.h"
int umi_market_data_snapshot_aligned(const UmiQuote *q,const UmiBar *b,int64_t tolerance_ms){if(q==NULL||b==NULL||tolerance_ms<0)return 0;int64_t delta=q->event_time_ms-b->end_time_ms;if(delta<0)delta=-delta;return delta<=tolerance_ms;}
