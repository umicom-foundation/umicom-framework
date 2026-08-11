/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/session.c
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

#include "umicom/integration/session.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_integration_session_init(
    UmiIntegrationSession *session,
    const char *session_id,
    const char *identity_id)
{
    int session_written;
    int identity_written;

    if (session == NULL || session_id == NULL || identity_id == NULL ||
        session_id[0] == '\0' || identity_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(session, 0, sizeof(*session));
    session_written = snprintf(session->session_id,
                               sizeof(session->session_id),
                               "%s",
                               session_id);
    identity_written = snprintf(session->identity_id,
                                sizeof(session->identity_id),
                                "%s",
                                identity_id);
    if (session_written < 0 || identity_written < 0 ||
        (size_t)session_written >= sizeof(session->session_id) ||
        (size_t)identity_written >= sizeof(session->identity_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    session->next_correlation = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_session_next_correlation(
    UmiIntegrationSession *session,
    char *output,
    size_t capacity)
{
    int written;
    if (session == NULL || output == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(output,
                       capacity,
                       "%s-%llu",
                       session->session_id,
                       (unsigned long long)session->next_correlation);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++session->next_correlation;
    return UMI_STATUS_OK;
}
