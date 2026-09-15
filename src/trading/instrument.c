/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/instrument.c
 *
 * PURPOSE:
 *   Validate canonical tradeable instrument records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of instrument. Product-specific UI and vendor details stay outside this file.
 */

#include <string.h>
#include <math.h>
#include "umicom/trading/instrument.h"
#include "umicom/finance/currency.h"
#include "umicom/finance/identifier.h"
/* Check that instrument satisfies its contract before another service relies on it. */
/*
 * Symbol and venue must terminate inside their own fields. A positive infinity
 * is not a usable multiplier. Identity comparison delegates to the existing
 * financial-identifier contract instead of duplicating an unsafe strcmp path.
 */
int umi_instrument_valid(const UmiInstrument *i){return i!=NULL && umi_financial_id_valid(&i->instrument_id) && i->symbol[0]!='\0' && memchr(i->symbol, '\0', sizeof(i->symbol)) != NULL && i->venue[0]!='\0' && memchr(i->venue, '\0', sizeof(i->venue)) != NULL && umi_currency_valid(&i->currency) && isfinite(i->multiplier) && i->multiplier>0.0;}
/* Provide the instrument same operation used by this module and its client applications. */
int umi_instrument_same(const UmiInstrument *a,const UmiInstrument *b){return a!=NULL&&b!=NULL&&umi_financial_id_equal(&a->instrument_id, &b->instrument_id);}
