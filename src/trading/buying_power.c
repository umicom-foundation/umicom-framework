/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/buying_power.c
 *
 * PURPOSE:
 *   Calculate simple cash-based buying power from leverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of buying power. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/buying_power.h"
/* Provide the buying power operation used by this module and its client applications. */
double umi_buying_power(double cash_value,double leverage){return cash_value>0.0&&leverage>0.0?cash_value*leverage:0.0;}
