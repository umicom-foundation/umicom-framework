/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/venue_adapter.c
 *
 * PURPOSE:
 *   Define a provider-neutral venue routing contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of venue adapter. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/venue_adapter.h"
/* Check that venue adapter satisfies its contract before another service relies on it. */
int umi_venue_adapter_valid(const UmiVenueAdapter *a){return a!=NULL&&a->instance!=NULL&&a->venue_name!=NULL&&a->route_order!=NULL;}
