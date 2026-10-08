/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/pnl.c
 * PURPOSE: Own account-scoped P&L streams, cancellation and freshness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static UmiIbkrPnlSnapshot *PnlFind(UmiIbkrConnection *c, uint32_t request)
{
    for (size_t i = 0U; i < UMI_IBKR_PNL_CAPACITY; ++i)
        if (request != 0U && c->pnl[i].requestId == request)
            return &c->pnl[i];
    return NULL;
}
UmiStatus UmiIbkrPnlSubscribe(UmiIbkrConnection *c, const UmiIbkrPnlSelection *selection, uint64_t now,
                              uint32_t *out)
{
    if (c == NULL || selection == NULL || out == NULL || now < c->lastNow ||
        selection->contractId > INT_MAX ||
        !UmiIbkrText(selection->account, sizeof selection->account, false) ||
        !UmiIbkrText(selection->modelCode, sizeof selection->modelCode, true))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    bool advertised = false;
    for (size_t i = 0U; i < c->snapshot.accountCount; ++i)
        if (!strcmp(c->snapshot.accounts[i], selection->account))
            advertised = true;
    if (!advertised)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiIbkrPnlSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_PNL_CAPACITY; ++i)
    {
        UmiIbkrPnlSnapshot *row = &c->pnl[i];
        if (!row->subscribed)
        {
            if (slot == NULL)
                slot = row;
            continue;
        }
        if (row->selection.contractId == selection->contractId &&
            !strcmp(row->selection.account, selection->account) &&
            !strcmp(row->selection.modelCode, selection->modelCode))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (slot == NULL || c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy scope first: it may be borrowed from a cancelled slot being reused. */
    UmiIbkrPnlSelection owned = *selection;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[24], contract[24];
    snprintf(id, sizeof id, "%" PRIu32, request);
    snprintf(contract, sizeof contract, "%" PRIu32, owned.contractId);
    const char *fields[] = {owned.contractId ? "94" : "92", id, owned.account, owned.modelCode, contract};
    UmiStatus status = UmiIbkrQueueFields(c, fields, owned.contractId ? 5U : 4U);
    if (status != UMI_STATUS_OK)
        return status;
    memset(slot, 0, sizeof *slot);
    slot->requestId = request;
    slot->selection = owned;
    slot->subscribed = true;
    slot->stale = true;
    slot->requestedAtMilliseconds = now;
    slot->revision = 1U;
    strcpy(slot->message, "Waiting for provider P&L; an absent response is not a zero balance.");
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPnlCancel(UmiIbkrConnection *c, uint32_t request)
{
    if (c == NULL || request == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrPnlSnapshot *slot = PnlFind(c, request);
    if (slot == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (!slot->subscribed)
        return UMI_STATUS_OK;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (slot->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char id[24];
    snprintf(id, sizeof id, "%" PRIu32, request);
    const char *fields[] = {slot->selection.contractId ? "95" : "93", id};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    slot->subscribed = false;
    slot->stale = true;
    ++slot->revision;
    strcpy(slot->message, "P&L subscription cancelled; retained figures are historical.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPnlCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t maximumAge,
                         UmiIbkrPnlSnapshot *out)
{
    if (c == NULL || request == 0U || out == NULL || maximumAge == 0U || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrPnlSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_PNL_CAPACITY; ++i)
        if (c->pnl[i].requestId == request)
            slot = &c->pnl[i];
    if (slot == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = *slot;
    out->stale = !slot->subscribed || slot->failed || !slot->received ||
                 c->snapshot.state != UMI_IBKR_READY || now < slot->receivedAtMilliseconds ||
                 now - slot->receivedAtMilliseconds > maximumAge;
    return UMI_STATUS_OK;
}
bool UmiIbkrPnlProviderMessage(UmiIbkrConnection *c, const char *text, int code, const char *message)
{
    (void)message;
    uint64_t request;
    if (!UmiIbkrUnsigned(text, &request) || request > UINT32_MAX)
        return false;
    UmiIbkrPnlSnapshot *slot = PnlFind(c, (uint32_t)request);
    if (slot == NULL || !slot->subscribed || slot->failed)
        return false;
    slot->failed = true;
    slot->stale = true;
    slot->providerCode = code;
    if (slot->revision != UINT64_MAX)
        ++slot->revision;
    snprintf(slot->message, sizeof slot->message, "Provider refused this P&L subscription (code %d).", code);
    return true;
}
