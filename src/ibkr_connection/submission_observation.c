/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/submission_observation.c
 *
 * PURPOSE:
 *   Retain broker status, rejection and transport uncertainty for locally submitted orders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdio.h>
#include <string.h>

static UmiIbkrOrderSubmission *Submitted(UmiIbkrConnection *c, uint32_t id)
{
    for (size_t i = 0U; i < c->submission.report.count; ++i)
        if (c->submission.report.orders[i].orderId == id)
            return &c->submission.report.orders[i];
    return NULL;
}
void UmiIbkrSubmissionObserve(UmiIbkrConnection *c, const UmiIbkrRecoveredOrder *observed)
{
    if (observed->key.clientId != c->options.adapter.clientId || observed->key.orderId < 0) return;
    UmiIbkrOrderSubmission *row = Submitted(c, (uint32_t)observed->key.orderId);
    if (!row) return;
    if (observed->hasOpenOrder && (strcmp(observed->open.account, row->draft.account) ||
        observed->open.contractId != row->draft.contractId)) {
        row->uncertain = true;
        strcpy(row->message, "Broker order identity differs from the submitted ticket; reconcile in Gateway.");
        UmiIbkrExecutionDisable(c); return;
    }
    /* A fresh matching open-order identity is sufficient to reconcile delivery.
     * A status alone lacks account/contract identity, so cannot clear uncertainty. */
    if (observed->hasOpenOrder) row->uncertain = false;
    row->brokerObserved = true;
    if (observed->hasStatus) {
        (void)snprintf(row->brokerStatus, sizeof row->brokerStatus, "%s", observed->status.status);
        /* Status is a broker observation, never a locally inferred fill.
         * Duplicate statuses replace the display and do not book cash or P&L. */
        row->filled = row->filled || !strcmp(observed->status.status, "Filled");
        row->cancelled = row->cancelled || !strcmp(observed->status.status, "Cancelled") ||
            !strcmp(observed->status.status, "ApiCancelled");
        (void)snprintf(row->message, sizeof row->message, "Broker status: %s; filled %s; remaining %s.",
            observed->status.status, observed->status.filled.reportedText,
            observed->status.remaining.reportedText);
    } else strcpy(row->message, "Broker open-order callback received; execution is not implied.");
}
bool UmiIbkrSubmissionMessage(UmiIbkrConnection *c, const char *request, int code, const char *text)
{
    uint64_t id;
    if (!UmiIbkrUnsigned(request, &id) || id > INT32_MAX) return false;
    UmiIbkrOrderSubmission *row = Submitted(c, (uint32_t)id);
    if (!row) return false;
    row->providerCode = code;
    (void)snprintf(row->message, sizeof row->message, "%s", text);
    /* Other codes may be warnings or cancellation refusals. Keep them visible
     * without inventing a terminal state or re-enabling automatic submission. */
    if (code == 201) row->rejected = true;
    if (code == 202) row->cancelled = true;
    return true;
}
void UmiIbkrSubmissionClosed(UmiIbkrConnection *c)
{
    UmiIbkrExecutionDisable(c);
    for (size_t i = 0U; i < c->submission.report.count; ++i) {
        UmiIbkrOrderSubmission *row = &c->submission.report.orders[i];
        if (row->filled || row->cancelled || row->rejected) continue;
        row->uncertain = true;
        strcpy(row->message, "Connection closed; reconcile this order in Gateway before creating another. No retry was sent.");
    }
}
