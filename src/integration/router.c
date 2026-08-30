/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/router.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/router.h"

#include <string.h>

void umi_integration_router_init(UmiIntegrationRouter *router)
{
    if (router != NULL) {
        (void)memset(router, 0, sizeof(*router));
    }
}

UmiStatus umi_integration_router_add(
    UmiIntegrationRouter *router,
    const UmiIntegrationRoute *route)
{
    if (router == NULL || route == NULL || route->topic[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (router->count >= UMI_INTEGRATION_MAX_ROUTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    router->routes[router->count++] = *route;
    return UMI_STATUS_OK;
}

const UmiIntegrationRoute *umi_integration_router_resolve(
    const UmiIntegrationRouter *router,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind)
{
    size_t index;
    if (router == NULL) {
        return NULL;
    }
    for (index = 0U; index < router->count; ++index) {
        if (umi_integration_route_matches(
                &router->routes[index],
                source_application,
                target_application,
                topic,
                kind)) {
            return &router->routes[index];
        }
    }
    return NULL;
}
