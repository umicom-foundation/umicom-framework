/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_execution.c
 *
 * PURPOSE:
 *   Bind reviewed limit orders to a connection, resolved instrument and explicitly authorised account.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <string.h>
#include <stdio.h>

static bool ExecutionAccount(const UmiIbkrConnection *c, const char *account)
{
    if (!UmiIbkrText(account, 64U, false) || strcmp(account, c->snapshot.selectedAccount))
        return false;
    for (size_t i = 0U; i < c->snapshot.accountCount; ++i)
        if (!strcmp(account, c->snapshot.accounts[i])) return true;
    return false;
}
static bool ExecutionReady(const UmiIbkrConnection *c)
{
    return c && c->snapshot.state == UMI_IBKR_READY && !c->snapshot.stale &&
        c->snapshot.summaryComplete && c->snapshot.positionsComplete &&
        c->snapshot.protocolVersion >= 164 && c->snapshot.protocolVersion <= 176;
}
/* Completed account responses are historical even on a healthy socket. Require
 * a recent explicit refresh for authority and each review/submission. */
static bool ExecutionAccountRecent(const UmiIbkrConnection *c, uint64_t now)
{
    return now >= c->snapshot.summaryAtMilliseconds &&
        now >= c->snapshot.positionsAtMilliseconds &&
        now - c->snapshot.summaryAtMilliseconds <= 60000U &&
        now - c->snapshot.positionsAtMilliseconds <= 60000U;
}
UmiStatus UmiIbkrExecutionEnable(UmiIbkrConnection *c, const char *account,
    UmiTradingEnvironment mode, bool confirmed)
{
    if (!c || !UmiIbkrText(account, 64U, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!confirmed || mode != c->options.environment || !ExecutionAccount(c, account))
        return UMI_STATUS_PERMISSION_DENIED;
    if (!ExecutionReady(c) || !ExecutionAccountRecent(c, c->lastNow)) return UMI_STATUS_INVALID_STATE;
    if (c->submission.report.enabled && strcmp(c->submission.report.account, account))
        return UMI_STATUS_INVALID_STATE;
    /* Reconnection never proves whether a queued order reached the broker.
     * Keep new submissions blocked while any non-terminal order is uncertain. */
    for (size_t i = 0U; i < c->submission.report.count; ++i) {
        const UmiIbkrOrderSubmission *row = &c->submission.report.orders[i];
        if (row->uncertain && !row->filled && !row->cancelled && !row->rejected)
            return UMI_STATUS_INVALID_STATE;
    }
    strcpy(c->submission.report.account, account);
    c->submission.report.enabled = true;
    c->snapshot.readOnly = false;
    return UMI_STATUS_OK;
}
void UmiIbkrExecutionDisable(UmiIbkrConnection *c)
{
    if (!c) return;
    c->submission.report.enabled = false;
    c->submission.report.reviewPending = false;
    c->snapshot.readOnly = true;
}
/* Price validation uses integer billionths, avoiding locale-dependent parsing
 * or binary floating-point rounding. Scientific notation and excess precision
 * are refused rather than silently converted at the trading boundary. */
static bool PriceUnits(const char *text, size_t capacity, uint64_t *units)
{
    if (!UmiIbkrText(text, capacity, false)) return false;
    uint64_t whole = 0U, fraction = 0U, factor = 100000000U;
    size_t at = 0U, digits = 0U;
    while (text[at] >= '0' && text[at] <= '9') {
        if (whole > 100000000U) return false;
        whole = whole * 10U + (unsigned)(text[at++] - '0');
        ++digits;
    }
    if (!digits || whole > 1000000000U) return false;
    if (text[at] == '.') {
        ++at; size_t start = at;
        while (text[at] >= '0' && text[at] <= '9') {
            if (!factor) return false;
            fraction += (unsigned)(text[at++] - '0') * factor;
            factor /= 10U;
        }
        if (at == start) return false;
    }
    if (text[at]) return false;
    *units = whole * 1000000000U + fraction;
    return *units != 0U;
}
static bool Listed(const char *list, const char *value)
{
    size_t length = strlen(value);
    for (const char *at = list; *at;) {
        const char *end = strchr(at, ',');
        size_t size = end ? (size_t)(end - at) : strlen(at);
        if (size == length && !memcmp(at, value, size)) return true;
        if (!end) break;
        at = end + 1;
    }
    return false;
}
UmiStatus UmiIbkrOrderPrepare(UmiIbkrConnection *c, const UmiIbkrOrderDraft *draft,
    uint64_t now, UmiIbkrOrderReview *out)
{
    if (!c) return UMI_STATUS_INVALID_ARGUMENT;
    /* An unsuccessful edited ticket must not leave its earlier review armed. */
    c->submission.report.reviewPending = false;
    if (!draft || !out || now < c->lastNow ||
        !UmiIbkrText(draft->exchange, sizeof draft->exchange, false) ||
        !UmiIbkrText(draft->account, sizeof draft->account, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->submission.report.enabled || !ExecutionAccount(c, draft->account) ||
        strcmp(c->submission.report.account, draft->account)) return UMI_STATUS_PERMISSION_DENIED;
    if (!ExecutionReady(c) || !ExecutionAccountRecent(c, now)) return UMI_STATUS_INVALID_STATE;
    if (!draft->contractId || draft->contractId > INT32_MAX ||
        !draft->quantity || draft->quantity > 1000000U) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiIbkrContractDetailsSnapshot *details = &c->contractDetails;
    if (!details->complete || details->failed || details->stale || details->count != 1U ||
        now < details->completedAtMilliseconds || now - details->completedAtMilliseconds > 60000U ||
        details->requestedContract.contractId != draft->contractId ||
        strcmp(details->requestedContract.exchange, draft->exchange))
        return UMI_STATUS_INVALID_STATE;
    const UmiIbkrContractDescription *instrument = &details->items[0];
    if (instrument->contractId != draft->contractId) return UMI_STATUS_INVALID_STATE;
    if ((strcmp(instrument->securityType, "STK") && strcmp(instrument->securityType, "FUT")) ||
        !strcmp(draft->exchange, "IBKRATS")) return UMI_STATUS_NOT_IMPLEMENTED;
    if (!Listed(instrument->validExchanges, draft->exchange) ||
        !Listed(instrument->orderTypes, "LMT")) return UMI_STATUS_PERMISSION_DENIED;
    uint64_t price, tick;
    if (!PriceUnits(draft->limitPrice, sizeof draft->limitPrice, &price) ||
        !PriceUnits(instrument->minimumTick, sizeof instrument->minimumTick, &tick) ||
        price % tick != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Minimum tick alone does not establish the increment at this price. Use
     * the existing route-specific band owner whenever the broker supplies it. */
    uint32_t ruleId = 0U;
    UmiStatus route = UmiIbkrContractRouteRule(details, 0U, draft->exchange, &ruleId);
    if (route == UMI_STATUS_OK) {
        UmiIbkrMarketRuleSnapshot rule = {0}; UmiDecimal exact;
        UmiTradingPriceIncrementReview increment = {0};
        if (UmiIbkrMarketRuleCopy(c, ruleId, now, &rule) != UMI_STATUS_OK ||
            !rule.complete || rule.stale || now < rule.receivedAtMilliseconds ||
            now - rule.receivedAtMilliseconds > 60000U ||
            UmiDecimalParseScientificExact(draft->limitPrice, strlen(draft->limitPrice), &exact) != UMI_STATUS_OK ||
            UmiTradingPriceIncrementReviewAt(rule.bands, rule.count, exact, &increment) != UMI_STATUS_OK ||
            !increment.aligned) return UMI_STATUS_INVALID_STATE;
    } else if (route != UMI_STATUS_UNAVAILABLE) return route;
    uint64_t minimum = 1000000000U, step = 1000000000U;
    if ((instrument->minimumSize[0] && !PriceUnits(instrument->minimumSize, sizeof instrument->minimumSize, &minimum)) ||
        (instrument->sizeIncrement[0] && !PriceUnits(instrument->sizeIncrement, sizeof instrument->sizeIncrement, &step)) ||
        (uint64_t)draft->quantity * 1000000000U < minimum ||
        ((uint64_t)draft->quantity * 1000000000U) % step) return UMI_STATUS_INVALID_ARGUMENT;
    if (c->submission.nextToken == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrOrderReview review = {0};
    review.token = ++c->submission.nextToken;
    review.createdAtMilliseconds = now;
    review.draft = *draft; review.instrument = *instrument;
    review.requestedEnvironment = c->options.environment;
    c->submission.review = review; c->submission.report.reviewPending = true;
    c->lastNow = now; *out = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrderSubmit(UmiIbkrConnection *c, uint64_t token, uint64_t now, uint32_t *outId)
{
    if (!c || !outId || !token || now < c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrExecutionStore *store = &c->submission;
    if (!store->report.enabled || !store->report.reviewPending || token != store->review.token ||
        !ExecutionAccount(c, store->review.draft.account) ||
        strcmp(store->report.account, store->review.draft.account)) return UMI_STATUS_PERMISSION_DENIED;
    if (!ExecutionReady(c) || !ExecutionAccountRecent(c, now) || now < store->review.createdAtMilliseconds ||
        now - store->review.createdAtMilliseconds > 30000U) {
        store->report.reviewPending = false;
        return UMI_STATUS_INVALID_STATE;
    }
    if (store->report.count == UMI_IBKR_SUBMISSION_LIMIT ||
        !c->orders.identity.brokerIdReceived || c->orders.identity.exhausted)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint32_t id = c->orders.identity.conservativeNextOrderId;
    /* Broker error callbacks share an integer namespace with read requests.
     * Reserve above every locally issued observation ID when ranges overlap;
     * later observations then advance past this order, preventing misrouting. */
    if (id >= 35001U && id < 36000U) id = 36000U;
    if (id >= 36000U && id < c->nextQuoteRequest) id = c->nextQuoteRequest;
    if (id >= INT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiIbkrOrderWire(c, &store->review, id);
    if (status != UMI_STATUS_OK) return status;
    UmiIbkrOrderSubmission row = {0};
    row.orderId = id; row.draft = store->review.draft; row.submittedAtMilliseconds = now;
    strcpy(row.message, "Queued for delivery; broker acceptance and fills are not yet known.");
    store->report.orders[store->report.count++] = row;
    store->report.reviewPending = false;
    UmiIbkrOrdersObserveId(c, (int32_t)id);
    if (id >= 36000U) c->nextQuoteRequest = id + 1U;
    c->lastNow = now; *outId = id;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrOrderCancelSubmitted(UmiIbkrConnection *c, uint32_t id, uint64_t now)
{
    if (!c || now < c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    /* Disabling new orders must not disable cancellation of this owner's orders.
     * Queueing cancellation is not evidence that an order can no longer fill. */
    if (c->snapshot.state != UMI_IBKR_READY || c->snapshot.stale) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < c->submission.report.count; ++i) {
        UmiIbkrOrderSubmission *row = &c->submission.report.orders[i];
        if (row->orderId != id) continue;
        if (row->cancelRequested || row->cancelled || row->filled || row->rejected)
            return UMI_STATUS_INVALID_STATE;
        UmiStatus status = UmiIbkrOrderCancelWire(c, id);
        if (status != UMI_STATUS_OK) return status;
        row->cancelRequested = true;
        strcpy(row->message, "Cancellation queued; the order can still fill until the broker confirms.");
        c->lastNow = now; return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiIbkrExecutionCopy(const UmiIbkrConnection *c, UmiIbkrExecutionReport *out)
{
    if (!c || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = c->submission.report;
    if (!ExecutionReady(c) || !ExecutionAccountRecent(c, c->lastNow) || !ExecutionAccount(c, out->account)) {
        out->enabled = false; out->reviewPending = false;
    }
    return UMI_STATUS_OK;
}
