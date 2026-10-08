/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_contract.c
 * PURPOSE: Resolve selected option definitions through the established contract-details owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "order_numbers.h"
#include "umicom/broker_connectivity/option_contract.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrOptionContractRequest(UmiIbkrConnection *c, const UmiIbkrOptionSelection *selection,
                                       uint64_t now, uint32_t *out)
{
    if (!c || !selection || !out || now < c->lastNow ||
        (selection->right != 'C' && selection->right != 'P') ||
        !UmiIbkrText(selection->currency, sizeof selection->currency, false) ||
        !UmiIbkrText(selection->exchange, sizeof selection->exchange, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 164 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiIbkrOptionChainSnapshot snapshot;
    UmiStatus status = UmiIbkrOptionChainCopy(c, selection->chainRequest, now, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (!snapshot.complete || snapshot.stale)
        return UMI_STATUS_INVALID_STATE;
    UmiIbkrOptionChainStore *store = c->optionChains;
    if (selection->chainIndex >= snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    const UmiIbkrOptionChain *chain = store->items[selection->chainIndex];
    if (selection->expiryIndex >= chain->expiryCount || selection->strikeIndex >= chain->strikeCount)
        return UMI_STATUS_NOT_FOUND;
    const UmiIbkrOrderNumber *strike = &chain->strikes[selection->strikeIndex];
    if (!strike->exact)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *type = NULL;
    if (!strcmp(snapshot.query.underlyingSecurityType, "FUT"))
        type = "FOP";
    else if (!strcmp(snapshot.query.underlyingSecurityType, "STK") ||
             !strcmp(snapshot.query.underlyingSecurityType, "IND"))
        type = "OPT";
    else
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiIbkrContractDetailsSnapshot *details = &c->contractDetails;
    if (details->requestId && !details->complete && !details->failed &&
        now - details->requestedAtMilliseconds < c->options.timeoutMilliseconds)
        return UMI_STATUS_BUSY;
    if (c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    UmiIbkrOptionIdentity identity = {0};
    strcpy(identity.symbol, snapshot.query.underlyingSymbol);
    strcpy(identity.securityType, type);
    strcpy(identity.expiry, chain->expirations[selection->expiryIndex]);
    strcpy(identity.strike, strike->reportedText);
    identity.right[0] = selection->right;
    strcpy(identity.multiplier, chain->multiplier);
    strcpy(identity.exchange, selection->exchange);
    strcpy(identity.currency, selection->currency);
    strcpy(identity.tradingClass, chain->tradingClass);
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"9",
                            "8",
                            id,
                            "0",
                            identity.symbol,
                            identity.securityType,
                            identity.expiry,
                            identity.strike,
                            identity.right,
                            identity.multiplier,
                            identity.exchange,
                            "",
                            identity.currency,
                            "",
                            identity.tradingClass,
                            "0",
                            "",
                            "",
                            ""};
    status = UmiIbkrQueueFields(c, fields, c->snapshot.protocolVersion >= 176 ? 19U : 18U);
    if (status != UMI_STATUS_OK)
        return status;
    memset(details, 0, sizeof *details);
    details->requestId = request;
    details->optionLookup = true;
    details->requestedOption = identity;
    strcpy(details->requestedContract.exchange, selection->exchange);
    details->requestedAtMilliseconds = now;
    strcpy(details->message, "Resolving one option candidate; wait for the completion marker.");
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    *out = request;
    return UMI_STATUS_OK;
}
bool UmiIbkrContractIdentityMatches(const UmiIbkrContractDetailsSnapshot *s,
                                    const UmiIbkrContractDescription *row)
{
    if (!s->optionLookup)
        return row->contractId == s->requestedContract.contractId;
    const UmiIbkrOptionIdentity *q = &s->requestedOption;
    UmiIbkrOrderNumber expected, actual, multiplier, reportedMultiplier;
    int strike = 1, multiple = 1;
    if (!UmiIbkrOrderNumberRead(q->strike, false, false, &expected) ||
        !UmiIbkrOrderNumberRead(row->strike, false, false, &actual) ||
        !UmiIbkrOrderNumberRead(q->multiplier, false, true, &multiplier) ||
        !UmiIbkrOrderNumberRead(row->multiplier, false, true, &reportedMultiplier) || !expected.exact ||
        !actual.exact || !multiplier.exact || !reportedMultiplier.exact ||
        UmiDecimalCompare(expected.value, actual.value, &strike) != UMI_STATUS_OK ||
        UmiDecimalCompare(multiplier.value, reportedMultiplier.value, &multiple) != UMI_STATUS_OK)
        return false;
    /* Route aliases may differ in broker descriptions. Identity is established
     * by the symbol, class, exact strike, expiry, right, currency and multiplier. */
    return !strcmp(q->symbol, row->symbol) && !strcmp(q->securityType, row->securityType) &&
           !strcmp(q->expiry, row->expiry) && !strcmp(q->right, row->right) &&
           !strcmp(q->currency, row->currency) && !strcmp(q->tradingClass, row->tradingClass) && !strike &&
           !multiple;
}
