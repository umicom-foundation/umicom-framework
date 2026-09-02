/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/price_band.c
 *
 * PURPOSE:
 *   Validate an order price against a reference-price percentage band.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of price band. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/price_band.h"
/* Provide the price within band operation used by this module and its client applications. */
int umi_price_within_band(double price,double reference,double fraction){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(price<=0.0||reference<=0.0||fraction<0.0)return 0;double diff=price-reference;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(diff<0.0)diff=-diff;return diff/reference<=fraction;}
