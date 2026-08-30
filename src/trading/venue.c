/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/venue.c
 *
 * PURPOSE:
 *   Validate venue identifiers used by routing and market-data services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of venue. Product-specific UI and vendor details stay outside this file.
 */

#include <string.h>
#include "umicom/trading/venue.h"
int umi_trading_venue_valid(const char *venue){return venue!=NULL && venue[0]!='\0' && strlen(venue)<UMI_TRADING_VENUE_CAPACITY;}
