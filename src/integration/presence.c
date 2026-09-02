/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/presence.c
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

#include "umicom/integration/presence.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise integration presence from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_presence_init(UmiIntegrationPresence *presence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presence != NULL) {
        (void)memset(presence, 0, sizeof(*presence));
    }
}

/*
 * Provide the integration presence heartbeat operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_presence_heartbeat(
    UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms,
    uint64_t lease_ms)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presence == NULL || application_id == NULL ||
        application_id[0] == '\0' || lease_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        UmiIntegrationPresenceRecord *record = &presence->records[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (record->used &&
            strcmp(record->application_id, application_id) == 0) {
            record->last_seen_ms = now_ms;
            record->lease_ms = lease_ms;
            return UMI_STATUS_OK;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        UmiIntegrationPresenceRecord *record = &presence->records[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!record->used) {
            int written = snprintf(record->application_id,
                                   sizeof(record->application_id),
                                   "%s",
                                   application_id);
            /* Apply this branch only when its contract condition is satisfied. */
            if (written < 0 ||
                (size_t)written >= sizeof(record->application_id)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            record->last_seen_ms = now_ms;
            record->lease_ms = lease_ms;
            record->used = true;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the integration presence is active operation used by this module and its client
 * applications.
 */
bool umi_integration_presence_is_active(
    const UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presence == NULL || application_id == NULL) {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        const UmiIntegrationPresenceRecord *record = &presence->records[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (record->used &&
            strcmp(record->application_id, application_id) == 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (now_ms < record->last_seen_ms) {
                return true;
            }
            return (now_ms - record->last_seen_ms) <= record->lease_ms;
        }
    }
    return false;
}
