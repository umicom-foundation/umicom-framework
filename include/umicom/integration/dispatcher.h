/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/dispatcher.h
 *
 * PURPOSE:
 *   Dispatch integration envelopes to registered public handlers.
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

#ifndef UMICOM_INTEGRATION_DISPATCHER_H
#define UMICOM_INTEGRATION_DISPATCHER_H

#include "umicom/base/status.h"
#include "umicom/integration/envelope.h"

typedef UmiStatus (*UmiIntegrationHandler)(
    const UmiIntegrationEnvelope *envelope,
    void *user_data);

typedef struct UmiIntegrationHandlerEntry {
    char target_application[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationHandler handler;
    void *user_data;
} UmiIntegrationHandlerEntry;

typedef struct UmiIntegrationDispatcher {
    UmiIntegrationHandlerEntry entries[UMI_INTEGRATION_MAX_HANDLERS];
    size_t count;
} UmiIntegrationDispatcher;

void umi_integration_dispatcher_init(UmiIntegrationDispatcher *dispatcher);
UmiStatus umi_integration_dispatcher_register(
    UmiIntegrationDispatcher *dispatcher,
    const char *target_application,
    const char *topic,
    UmiIntegrationHandler handler,
    void *user_data);
UmiStatus umi_integration_dispatcher_dispatch(
    const UmiIntegrationDispatcher *dispatcher,
    const UmiIntegrationEnvelope *envelope);

#endif
