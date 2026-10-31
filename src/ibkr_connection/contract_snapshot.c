/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/contract_snapshot.c
 *
 * PURPOSE:
 *   Copy instrument metadata with bounded observation age and unchanged-output guarantees.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>

/* A copy is owned evidence, not a live reference. Keep the established API for
 * callers that need its original semantics; displays can request a bounded age
 * without mutating the session or extending freshness when they refresh. */
UmiStatus UmiIbkrContractDetailsCopyWithAge(const UmiIbkrConnection *connection,
    uint32_t request, uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
    UmiIbkrContractDetailsSnapshot *out)
{
    if (connection == NULL || request == 0U || out == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrContractDetailsSnapshot *copy = malloc(sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiIbkrContractDetailsCopy(connection, request, nowMilliseconds, copy);
    if (status == UMI_STATUS_OK && copy->count > UMI_IBKR_CONTRACT_DETAILS_LIMIT)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        uint64_t observed = copy->complete ? copy->completedAtMilliseconds : copy->requestedAtMilliseconds;
        if (observed > nowMilliseconds ||
            (copy->complete && observed < copy->requestedAtMilliseconds))
            status = UMI_STATUS_INVALID_ARGUMENT;
        else {
            copy->stale = copy->stale || connection->snapshot.stale ||
                nowMilliseconds - observed >= maximumAgeMilliseconds;
            *out = *copy;
        }
    }
    free(copy);
    return status;
}

/* Exact-ID and option resolution requests replace the same connection-owned
 * description. Read that owner directly so callers do not guess which native
 * form most recently advanced its request ID. */
UmiStatus UmiIbkrContractDetailsCopyLatestWithAge(const UmiIbkrConnection *connection,
    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds, UmiIbkrContractDetailsSnapshot *out)
{
    if (connection == NULL || out == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->contractDetails.requestId == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrContractDetailsCopyWithAge(connection, connection->contractDetails.requestId,
        nowMilliseconds, maximumAgeMilliseconds, out);
}
