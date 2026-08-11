/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/presence.c
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

#include "umicom/integration/presence.h"

#include <stdio.h>
#include <string.h>

void umi_integration_presence_init(UmiIntegrationPresence *presence)
{
    if (presence != NULL) {
        (void)memset(presence, 0, sizeof(*presence));
    }
}

UmiStatus umi_integration_presence_heartbeat(
    UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms,
    uint64_t lease_ms)
{
    size_t index;
    if (presence == NULL || application_id == NULL ||
        application_id[0] == '\0' || lease_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        UmiIntegrationPresenceRecord *record = &presence->records[index];
        if (record->used &&
            strcmp(record->application_id, application_id) == 0) {
            record->last_seen_ms = now_ms;
            record->lease_ms = lease_ms;
            return UMI_STATUS_OK;
        }
    }

    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        UmiIntegrationPresenceRecord *record = &presence->records[index];
        if (!record->used) {
            int written = snprintf(record->application_id,
                                   sizeof(record->application_id),
                                   "%s",
                                   application_id);
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

bool umi_integration_presence_is_active(
    const UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms)
{
    size_t index;
    if (presence == NULL || application_id == NULL) {
        return false;
    }
    for (index = 0U; index < UMI_INTEGRATION_MAX_APPLICATIONS; ++index) {
        const UmiIntegrationPresenceRecord *record = &presence->records[index];
        if (record->used &&
            strcmp(record->application_id, application_id) == 0) {
            if (now_ms < record->last_seen_ms) {
                return true;
            }
            return (now_ms - record->last_seen_ms) <= record->lease_ms;
        }
    }
    return false;
}
