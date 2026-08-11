/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/catalogue.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/catalogue.h"

static const char *CAPABILITIES[] = {
    "umicom.integration.discovery/1",
    "umicom.integration.routing/1",
    "umicom.integration.session/1",
    "umicom.integration.suite-runtime/1",
    "umicom.integration.launch-plan/1",
    "umicom.integration.presence/1"
};

size_t umi_integration_catalogue_count(void)
{
    return sizeof(CAPABILITIES) / sizeof(CAPABILITIES[0]);
}

const char *umi_integration_catalogue_at(size_t index)
{
    return index < umi_integration_catalogue_count()
        ? CAPABILITIES[index]
        : NULL;
}
