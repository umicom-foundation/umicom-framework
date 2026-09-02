/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/self_trade.c
 *
 * PURPOSE:
 *   Detect opposite-side orders for the same account and instrument.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of self trade. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/self_trade.h"
#include "umicom/finance/identifier.h"
#include "umicom/trading/instrument.h"
/* Provide the self trade risk operation used by this module and its client applications. */
int umi_self_trade_risk(const UmiOrderRequest *a,const UmiOrderRequest *b){return a!=NULL&&b!=NULL&&a->side!=b->side&&umi_financial_id_equal(&a->account_id,&b->account_id)&&umi_instrument_same(&a->instrument,&b->instrument);}
