/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/news_providers.c
 *
 * PURPOSE:
 *   Capture subscribed news providers through the existing read-only broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include <string.h>

UmiStatus UmiIbkrNewsProvidersRequest(UmiIbkrConnection *c, uint64_t now)
{
    if (c == NULL || now < c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY) return UMI_STATUS_INVALID_STATE;
    /* The transport negotiates the bounded legacy field protocol. Do not
     * silently claim support for future protobuf envelopes. */
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (c->newsProviders.requested) return UMI_STATUS_BUSY;
    const char *fields[] = {"85"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 1U);
    if (status != UMI_STATUS_OK) return status;
    c->newsProviders.requested = true;
    c->newsProviders.requestedAtMilliseconds = now;
    c->lastNow = now;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrNewsProvidersCopy(const UmiIbkrConnection *c, uint64_t now,
    UmiIbkrNewsProvidersSnapshot *out)
{
    if (c == NULL || out == NULL || now < c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    *out = c->newsProviders;
    out->timedOut = out->requested && !out->complete &&
        now - out->requestedAtMilliseconds >= c->options.timeoutMilliseconds;
    out->stale = out->timedOut || c->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrNewsProvidersFrame(UmiIbkrConnection *c, const unsigned char *body,
    size_t length, uint64_t now)
{
    /* Unsolicited, repeated and expired captures never become fresh results. */
    if (now < c->newsProviders.requestedAtMilliseconds || !c->newsProviders.requested || c->newsProviders.complete ||
        c->snapshot.state != UMI_IBKR_READY ||
        now - c->newsProviders.requestedAtMilliseconds >= c->options.timeoutMilliseconds)
        return UmiIbkrObservationIgnore(c);
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length,
        2U + 2U * UMI_IBKR_NEWS_PROVIDER_LIMIT, &fields);
    if (status != UMI_STATUS_OK) return status;
    uint64_t count = 0U;
    UmiIbkrNewsProvidersSnapshot candidate = c->newsProviders;
    if (fields.count < 2U || strcmp(fields.values[0], "85") ||
        !UmiIbkrUnsigned(fields.values[1], &count) ||
        count > UMI_IBKR_NEWS_PROVIDER_LIMIT || fields.count != 2U + (size_t)count * 2U)
        status = UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < (size_t)count; ++i) {
        const char *code = fields.values[2U + 2U * i];
        const char *name = fields.values[3U + 2U * i];
        if (!UmiIbkrText(code, sizeof candidate.providers[i].code, false) ||
            !UmiIbkrText(name, sizeof candidate.providers[i].name, false)) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        for (size_t j = 0U; j < i; ++j)
            if (!strcmp(candidate.providers[j].code, code)) status = UMI_STATUS_PARSE_ERROR;
        if (status != UMI_STATUS_OK) break;
        strcpy(candidate.providers[i].code, code);
        strcpy(candidate.providers[i].name, name);
    }
    if (status == UMI_STATUS_OK) {
        candidate.count = (size_t)count;
        candidate.complete = true;
        candidate.receivedAtMilliseconds = now;
        /* Atomic publication: a malformed later row cannot expose partial data. */
        c->newsProviders = candidate;
    }
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
