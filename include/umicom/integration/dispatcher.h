/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/dispatcher.h
 *
 * PURPOSE:
 *   Dispatch integration envelopes to registered public handlers.
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

#ifndef UMICOM_INTEGRATION_DISPATCHER_H
#define UMICOM_INTEGRATION_DISPATCHER_H

#include "umicom/base/status.h"
#include "umicom/integration/envelope.h"

typedef UmiStatus (*UmiIntegrationHandler)(
    const UmiIntegrationEnvelope *envelope,
    void *user_data);

/**
 * Represent the integration handler entry data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationHandlerEntry {
    char target_application[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationHandler handler;
    void *user_data;
} UmiIntegrationHandlerEntry;

/**
 * Represent the integration dispatcher data shared with callers of this public contract.
 */
typedef struct UmiIntegrationDispatcher {
    UmiIntegrationHandlerEntry entries[UMI_INTEGRATION_MAX_HANDLERS];
    size_t count;
} UmiIntegrationDispatcher;

/**
 * Initialise integration dispatcher from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_dispatcher_init(UmiIntegrationDispatcher *dispatcher);
/**
 * Add integration dispatcher only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_dispatcher_register(
    UmiIntegrationDispatcher *dispatcher,
    const char *target_application,
    const char *topic,
    UmiIntegrationHandler handler,
    void *user_data);
/**
 * Perform integration dispatcher through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_integration_dispatcher_dispatch(
    const UmiIntegrationDispatcher *dispatcher,
    const UmiIntegrationEnvelope *envelope);

#endif
