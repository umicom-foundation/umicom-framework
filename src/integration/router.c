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

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/router.h"

#include <string.h>

/*
 * Initialise integration router from caller-provided values so later operations receive a
 * known state.
 */
void umi_integration_router_init(UmiIntegrationRouter *router)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router != NULL) {
        (void)memset(router, 0, sizeof(*router));
    }
}

/* Add integration router only after its inputs and available capacity have been checked. */
UmiStatus umi_integration_router_add(
    UmiIntegrationRouter *router,
    const UmiIntegrationRoute *route)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL || route == NULL || route->topic[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (router->count >= UMI_INTEGRATION_MAX_ROUTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    router->routes[router->count++] = *route;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration router resolve operation used by this module and its client
 * applications.
 */
const UmiIntegrationRoute *umi_integration_router_resolve(
    const UmiIntegrationRouter *router,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (router == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < router->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
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
