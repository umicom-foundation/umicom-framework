/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/presence.h
 *
 * PURPOSE:
 *   Track bounded application heartbeats and lease expiry.
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

#ifndef UMICOM_INTEGRATION_PRESENCE_H
#define UMICOM_INTEGRATION_PRESENCE_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

/**
 * Represent the integration presence record data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationPresenceRecord {
    char application_id[UMI_INTEGRATION_ID_CAPACITY];
    uint64_t last_seen_ms;
    uint64_t lease_ms;
    bool used;
} UmiIntegrationPresenceRecord;

/**
 * Represent the integration presence data shared with callers of this public contract.
 */
typedef struct UmiIntegrationPresence {
    UmiIntegrationPresenceRecord records[UMI_INTEGRATION_MAX_APPLICATIONS];
} UmiIntegrationPresence;

/**
 * Initialise integration presence from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_presence_init(UmiIntegrationPresence *presence);
/**
 * Provide the integration presence heartbeat operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_presence_heartbeat(
    UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms,
    uint64_t lease_ms);
/**
 * Provide the integration presence is active operation used by this module and its client
 * applications.
 */
bool umi_integration_presence_is_active(
    const UmiIntegrationPresence *presence,
    const char *application_id,
    uint64_t now_ms);

#endif
