/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/bridge.h
 *
 * PURPOSE:
 *   Define narrow callbacks used by platform hosts to launch, stop and send without hard product linkage.
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

#ifndef UMICOM_INTEGRATION_BRIDGE_H
#define UMICOM_INTEGRATION_BRIDGE_H

#include "umicom/base/status.h"
#include "umicom/integration/envelope.h"

typedef UmiStatus (*UmiIntegrationLaunchFn)(
    const char *application_id,
    void *user_data);
typedef UmiStatus (*UmiIntegrationStopFn)(
    const char *application_id,
    void *user_data);
typedef UmiStatus (*UmiIntegrationSendFn)(
    const UmiIntegrationEnvelope *envelope,
    void *user_data);

typedef struct UmiIntegrationBridge {
    UmiIntegrationLaunchFn launch;
    UmiIntegrationStopFn stop;
    UmiIntegrationSendFn send;
    void *user_data;
} UmiIntegrationBridge;

bool umi_integration_bridge_valid(const UmiIntegrationBridge *bridge);

#endif
