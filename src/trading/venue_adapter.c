/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/venue_adapter.c
 *
 * PURPOSE:
 *   Define a provider-neutral venue routing contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of venue adapter. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/venue_adapter.h"
int umi_venue_adapter_valid(const UmiVenueAdapterV1 *a){return a!=NULL&&a->instance!=NULL&&a->venue_name!=NULL&&a->route_order!=NULL;}
