/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/route.h
 *
 * PURPOSE:
 *   Define one typed Integration Fabric route between applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_ROUTE_H
#define UMICOM_INTEGRATION_ROUTE_H

#include "umicom/integration/types.h"

typedef struct UmiIntegrationRoute {
    char source_application[UMI_INTEGRATION_ID_CAPACITY];
    char target_application[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationMessageKind kind;
    bool target_required;
} UmiIntegrationRoute;

bool umi_integration_route_matches(
    const UmiIntegrationRoute *route,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind);

#endif
