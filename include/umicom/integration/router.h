/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/router.h
 *
 * PURPOSE:
 *   Store and resolve public Integration Fabric routes.
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

#ifndef UMICOM_INTEGRATION_ROUTER_H
#define UMICOM_INTEGRATION_ROUTER_H

#include "umicom/base/status.h"
#include "umicom/integration/route.h"

/**
 * Represent the integration router data shared with callers of this public contract.
 */
typedef struct UmiIntegrationRouter {
    UmiIntegrationRoute routes[UMI_INTEGRATION_MAX_ROUTES];
    size_t count;
} UmiIntegrationRouter;

/**
 * Initialise integration router from caller-provided values so later operations receive a
 * known state.
 */
void umi_integration_router_init(UmiIntegrationRouter *router);
/**
 * Add integration router only after its inputs and available capacity have been checked.
 */
UmiStatus umi_integration_router_add(
    UmiIntegrationRouter *router,
    const UmiIntegrationRoute *route);
/**
 * Provide the integration router resolve operation used by this module and its client
 * applications.
 */
const UmiIntegrationRoute *umi_integration_router_resolve(
    const UmiIntegrationRouter *router,
    const char *source_application,
    const char *target_application,
    const char *topic,
    UmiIntegrationMessageKind kind);

#endif
