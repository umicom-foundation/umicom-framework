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

/* BEGINNER NOTE:
 * This source implements the small deterministic core of price band. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/price_band.h"
int umi_price_within_band(double price,double reference,double fraction){if(price<=0.0||reference<=0.0||fraction<0.0)return 0;double diff=price-reference;if(diff<0.0)diff=-diff;return diff/reference<=fraction;}
