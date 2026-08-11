/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/buying_power.c
 *
 * PURPOSE:
 *   Calculate simple cash-based buying power from leverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of buying power. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/buying_power.h"
double umi_buying_power(double cash_value,double leverage){return cash_value>0.0&&leverage>0.0?cash_value*leverage:0.0;}
