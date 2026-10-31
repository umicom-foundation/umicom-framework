/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_snapshot.c
 *
 * PURPOSE:
 *   Copy option definitions with bounded age, storage checks and explicit selection identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/option_contract.h"
#include <stdlib.h>

/* Existing copy calls remain available. Workspaces need a stricter boundary:
 * a completed definition has an observation age and cannot be kept fresh merely
 * by reading it again. Failed copies never replace the caller's last evidence. */
UmiStatus UmiIbkrOptionChainCopyWithAge(const UmiIbkrConnection *connection,
    uint32_t request, uint64_t now, uint64_t maximumAge, UmiIbkrOptionChainSnapshot *out)
{
    if (connection == NULL || request == 0U || out == NULL || maximumAge == 0U ||
        now < connection->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->optionChains == NULL ||
        connection->optionChains->snapshot.requestId != request) return UMI_STATUS_NOT_FOUND;
    const UmiIbkrOptionChainSnapshot *stored = &connection->optionChains->snapshot;
    if (stored->count > UMI_IBKR_OPTION_CHAIN_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t observed = stored->complete ? stored->completedAtMilliseconds : stored->requestedAtMilliseconds;
    if (stored->requestedAtMilliseconds > now || observed > now ||
        (stored->complete && observed < stored->requestedAtMilliseconds)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOptionChainSnapshot copy;
    UmiStatus status = UmiIbkrOptionChainCopy(connection, request, now, &copy);
    if (status == UMI_STATUS_OK) {
        copy.stale = copy.stale || connection->snapshot.stale || now - observed >= maximumAge;
        *out = copy;
    }
    return status;
}
UmiStatus UmiIbkrOptionChainCopyLatestWithAge(const UmiIbkrConnection *connection,
    uint64_t now, uint64_t maximumAge, UmiIbkrOptionChainSnapshot *out)
{
    if (connection == NULL || out == NULL || maximumAge == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (connection->optionChains == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrOptionChainCopyWithAge(connection, connection->optionChains->snapshot.requestId,
        now, maximumAge, out);
}

/* Array indices keep their provider meaning. Never compact a list after a
 * failed item read: a displayed index must still resolve to the same chain. */
UmiStatus UmiIbkrOptionChainItemCopyChecked(const UmiIbkrConnection *connection,
    uint32_t request, size_t index, UmiIbkrOptionChain *out)
{
    if (connection == NULL || request == 0U || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrOptionChainStore *store = connection->optionChains;
    if (store == NULL || store->snapshot.requestId != request) return UMI_STATUS_NOT_FOUND;
    if (store->snapshot.count > UMI_IBKR_OPTION_CHAIN_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (index >= store->snapshot.count) return UMI_STATUS_NOT_FOUND;
    const UmiIbkrOptionChain *chain = store->items[index];
    if (chain == NULL) return UMI_STATUS_UNAVAILABLE;
    if (chain->expiryCount > UMI_IBKR_OPTION_EXPIRY_LIMIT || chain->strikeCount > UMI_IBKR_OPTION_STRIKE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (chain->underlyingContractId != store->snapshot.query.underlyingContractId ||
        !UmiIbkrText(chain->exchange, sizeof(chain->exchange), false) ||
        !UmiIbkrText(chain->tradingClass, sizeof(chain->tradingClass), true) ||
        !UmiIbkrText(chain->multiplier, sizeof(chain->multiplier), true))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < chain->expiryCount; ++i)
        if (!UmiIbkrText(chain->expirations[i], sizeof(chain->expirations[i]), false))
            return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < chain->strikeCount; ++i)
        if (!UmiIbkrText(chain->strikes[i].reportedText, sizeof(chain->strikes[i].reportedText), false))
            return UMI_STATUS_INVALID_ARGUMENT;
    *out = *chain;
    return UMI_STATUS_OK;
}

/* Resolve one reviewed candidate, not the Cartesian product of every expiry
 * and strike. The response is still evidence to inspect, not an order or quote.
 * A separate additive API retains the established request semantics for callers
 * that intentionally manage age themselves. */
UmiStatus UmiIbkrOptionContractRequestWithAge(UmiIbkrConnection *connection,
    const UmiIbkrOptionSelection *selection, uint64_t now, uint64_t maximumAge, uint32_t *outRequest)
{
    if (connection == NULL || selection == NULL || outRequest == NULL || maximumAge == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOptionChainSnapshot snapshot;
    UmiStatus status = UmiIbkrOptionChainCopyWithAge(connection, selection->chainRequest, now, maximumAge, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.complete || snapshot.stale) return UMI_STATUS_INVALID_STATE;
    UmiIbkrOptionChain *chain = malloc(sizeof(*chain));
    if (chain == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiIbkrOptionChainItemCopyChecked(connection, selection->chainRequest, selection->chainIndex, chain);
    if (status == UMI_STATUS_OK &&
        (selection->expiryIndex >= chain->expiryCount || selection->strikeIndex >= chain->strikeCount))
        status = UMI_STATUS_INVALID_ARGUMENT;
    free(chain);
    if (status != UMI_STATUS_OK) return status;
    return UmiIbkrOptionContractRequest(connection, selection, now, outRequest);
}
