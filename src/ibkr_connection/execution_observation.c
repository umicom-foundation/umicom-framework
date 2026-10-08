/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_observation.c
 * PURPOSE: Own a bounded, account-scoped execution capture with explicit completion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
static bool ExecutionsPending(const UmiIbkrConnection *c, uint64_t now)
{
    const UmiIbkrExecutionSnapshot *s = &c->executions;
    return s->requestId != 0U && !s->complete && !s->failed && now >= s->requestedAtMilliseconds &&
           now - s->requestedAtMilliseconds < c->options.timeoutMilliseconds;
}
UmiStatus UmiIbkrExecutionsRequest(UmiIbkrConnection *c, const char *account, uint64_t now,
                                   uint32_t *outRequest)
{
    if (c == NULL || outRequest == NULL || !UmiIbkrText(account, 64U, false) || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 164 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    bool allowed = false;
    for (size_t i = 0U; i < c->snapshot.accountCount; ++i)
        if (strcmp(account, c->snapshot.accounts[i]) == 0)
            allowed = true;
    if (!allowed)
        return UMI_STATUS_PERMISSION_DENIED;
    if (ExecutionsPending(c, now))
        return UMI_STATUS_BUSY;
    uint32_t request = c->nextQuoteRequest == 0U ? 36000U : c->nextQuoteRequest;
    if (request >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char id[24], selected[64];
    snprintf(id, sizeof id, "%u", (unsigned)request);
    /* Account may point into the previous capture. Copy it before replacement. */
    snprintf(selected, sizeof selected, "%s", account);
    const char *fields[] = {"7", "3", id, "0", selected, "", "", "", "", ""};
    UmiStatus status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK)
        return status;
    memset(&c->executions, 0, sizeof c->executions);
    c->executions.requestId = request;
    strcpy(c->executions.account, selected);
    c->executions.requestedAtMilliseconds = now;
    c->executions.revision = 1U;
    strcpy(c->executions.message,
           "Waiting for the execution end marker; this is a recent capture, not complete account history.");
    c->lastNow = now;
    c->nextQuoteRequest = request + 1U;
    *outRequest = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrExecutionsCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now,
                                UmiIbkrExecutionSnapshot *out)
{
    if (c == NULL || out == NULL || request == 0U || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->executions.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    *out = c->executions;
    if (!out->complete && !out->failed &&
        now - out->requestedAtMilliseconds >= c->options.timeoutMilliseconds)
    {
        out->failed = true;
        strcpy(out->message, "Execution capture timed out; received rows remain incomplete.");
    }
    out->stale = out->failed || c->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrExecutionsAbandon(UmiIbkrConnection *c, uint32_t request)
{
    if (c == NULL || request == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->executions.requestId != request)
        return UMI_STATUS_NOT_FOUND;
    if (c->executions.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    c->executions.failed = true;
    ++c->executions.revision;
    strcpy(c->executions.message, "Local execution capture abandoned; orders are unchanged.");
    return UMI_STATUS_OK;
}
bool UmiIbkrExecutionsProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *message)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request != c->executions.requestId || c->executions.complete ||
        c->executions.failed)
        return false;
    c->executions.failed = true;
    if (c->executions.revision != UINT64_MAX)
        ++c->executions.revision;
    /* Keep provider text out of the capture: the connection already validates
     * it, but it may contain account details unrelated to this selected view. */
    (void)message;
    snprintf(c->executions.message, sizeof c->executions.message,
             "Provider refused the execution capture (code %d). Review TWS diagnostics.", code);
    return true;
}
bool UmiIbkrExecutionsAccepting(const UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    return c->snapshot.state == UMI_IBKR_READY && c->executions.requestId == request &&
           ExecutionsPending(c, now);
}
