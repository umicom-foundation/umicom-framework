/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/session.c
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

#include "umicom/integration/session.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/*
 * Initialise integration session from caller-provided values so later operations receive a
 * known state.
 */
/* Keep the original direct writes for review. Failed admission previously
 * replaced an established session with a partly copied identity. */
#if 0
UmiStatus umi_integration_session_init(
    UmiIntegrationSession *session,
    const char *session_id,
    const char *identity_id)
{
    int session_written;
    int identity_written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (session_written < 0 || identity_written < 0 ||
        (size_t)session_written >= sizeof(session->session_id) ||
        (size_t)identity_written >= sizeof(session->identity_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    session->next_correlation = 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_integration_session_init(UmiIntegrationSession *session,
    const char *session_id, const char *identity_id)
{
    if (session == NULL || session_id == NULL || identity_id == NULL ||
        session_id[0] == '\0' || identity_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Assemble both identities before publishing the replacement. The
     * caller may use existing session strings when creating a new session. */
    UmiIntegrationSession candidate = {0};
    int session_written = snprintf(candidate.session_id, sizeof candidate.session_id, "%s", session_id);
    int identity_written = snprintf(candidate.identity_id, sizeof candidate.identity_id, "%s", identity_id);
    if (session_written < 0 || identity_written < 0 ||
        (size_t)session_written >= sizeof candidate.session_id ||
        (size_t)identity_written >= sizeof candidate.identity_id)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    candidate.next_correlation = 1U;
    *session = candidate;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration session next correlation operation used by this module and its
 * client applications.
 */
/* Keep the original formatter for review. It exposed partial IDs on a short
 * buffer and allowed the sequence to wrap back to earlier values. */
#if 0
UmiStatus umi_integration_session_next_correlation(
    UmiIntegrationSession *session,
    char *output,
    size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || output == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(output,
                       capacity,
                       "%s-%llu",
                       session->session_id,
                       (unsigned long long)session->next_correlation);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++session->next_correlation;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_integration_session_next_correlation(UmiIntegrationSession *session,
    char *output, size_t capacity)
{
    if (session == NULL || output == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The public session holds a fixed-size string. Validate that bound
     * before passing caller-owned state to the C library's text formatter. */
    if (session->session_id[0] == '\0' ||
        memchr(session->session_id, '\0', sizeof session->session_id) == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (session->next_correlation == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Reserve space for the longest accepted session name, a hyphen, up to
     * 20 decimal digits and the terminator. Commit only a complete ID. */
    char candidate[UMI_INTEGRATION_ID_CAPACITY + 21U];
    int written = snprintf(candidate, sizeof candidate, "%s-%" PRIu64,
        session->session_id, session->next_correlation);
    if (written < 0 || (size_t)written >= sizeof candidate || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(output, candidate, (size_t)written + 1U);
    /* Zero is the exhausted marker. Continue to issue UINT64_MAX exactly
     * once so the last identifier can complete its ordinary lifecycle. */
    session->next_correlation = session->next_correlation == UINT64_MAX ? 0U : session->next_correlation + 1U;
    return UMI_STATUS_OK;
}
