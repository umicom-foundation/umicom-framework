/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/surveillance.c
 *
 * PURPOSE:
 *   Flag unusually large orders relative to a configured threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of surveillance. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/surveillance.h"
/*
 * Provide the surveillance large order operation used by this module and its client
 * applications.
 */
int umi_surveillance_large_order(const UmiOrderRequest *r,double threshold){return r!=NULL&&threshold>0.0&&r->quantity>=threshold;}
