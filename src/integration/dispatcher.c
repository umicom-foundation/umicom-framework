/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/dispatcher.c
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

#include "umicom/integration/dispatcher.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise integration dispatcher from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_dispatcher_init(
    UmiIntegrationDispatcher *dispatcher)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dispatcher != NULL) {
        (void)memset(dispatcher, 0, sizeof(*dispatcher));
    }
}

/*
 * Add integration dispatcher only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_integration_dispatcher_register(
    UmiIntegrationDispatcher *dispatcher,
    const char *target_application,
    const char *topic,
    UmiIntegrationHandler handler,
    void *user_data)
{
    UmiIntegrationHandlerEntry *entry;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dispatcher == NULL || target_application == NULL ||
        topic == NULL || handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (dispatcher->count >= UMI_INTEGRATION_MAX_HANDLERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &dispatcher->entries[dispatcher->count];
    written = snprintf(entry->target_application,
                       sizeof(entry->target_application),
                       "%s",
                       target_application);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0 ||
        (size_t)written >= sizeof(entry->target_application)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(entry->topic, sizeof(entry->topic), "%s", topic);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(entry->topic)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry->handler = handler;
    entry->user_data = user_data;
    ++dispatcher->count;
    return UMI_STATUS_OK;
}

/*
 * Perform integration dispatcher through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_integration_dispatcher_dispatch(
    const UmiIntegrationDispatcher *dispatcher,
    const UmiIntegrationEnvelope *envelope)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dispatcher == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_integration_envelope_validate(envelope);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < dispatcher->count; ++index) {
        const UmiIntegrationHandlerEntry *entry = &dispatcher->entries[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if ((strcmp(entry->target_application,
                    envelope->target_application) == 0 ||
             strcmp(entry->target_application, "*") == 0) &&
            (strcmp(entry->topic, envelope->topic) == 0 ||
             strcmp(entry->topic, "*") == 0)) {
            return entry->handler(envelope, entry->user_data);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
