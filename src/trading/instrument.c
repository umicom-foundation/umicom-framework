/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/instrument.c
 *
 * PURPOSE:
 *   Validate canonical tradeable instrument records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of instrument. Product-specific UI and vendor details stay outside this file.
 */

#include <string.h>
#include "umicom/trading/instrument.h"
#include "umicom/finance/currency.h"
#include "umicom/finance/identifier.h"
int umi_instrument_valid(const UmiInstrument *i){return i!=NULL && umi_financial_id_valid(&i->instrument_id) && i->symbol[0]!='\0' && i->venue[0]!='\0' && umi_currency_valid(&i->currency) && i->multiplier>0.0;}
int umi_instrument_same(const UmiInstrument *a,const UmiInstrument *b){return a!=NULL&&b!=NULL&&strcmp(a->instrument_id.value,b->instrument_id.value)==0;}
