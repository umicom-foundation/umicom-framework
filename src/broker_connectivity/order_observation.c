/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order_observation.c
 * PURPOSE: Classify IBKR status and cumulative quantity observations without guessing final outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/order_observation.h"
#include <string.h>

UmiStatus UmiIbkrClassifyOrderPhase(const char *text, UmiIbkrOrderPhase *out)
{
    if (text == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    for (; length < 32U; ++length)
    {
        unsigned char character = (unsigned char)text[length];
        if (character == 0U)
            break;
        if (character < 33U || character > 126U)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U || length == 32U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep this table tied to the provider meanings. In particular, do not
     * translate a cancellation request into a confirmed terminal outcome. */
    static const struct
    {
        const char *text;
        UmiIbkrOrderPhase phase;
    } names[] = {{"Inactive", UMI_IBKR_ORDER_INACTIVE},
                 {"PendingSubmit", UMI_IBKR_ORDER_PENDING_SUBMIT},
                 {"PreSubmitted", UMI_IBKR_ORDER_PRE_SUBMITTED},
                 {"Submitted", UMI_IBKR_ORDER_WORKING},
                 {"PendingCancel", UMI_IBKR_ORDER_PENDING_CANCEL},
                 {"PreCancelled", UMI_IBKR_ORDER_PENDING_CANCEL},
                 {"Cancelled", UMI_IBKR_ORDER_CANCELLED},
                 {"ApiCancelled", UMI_IBKR_ORDER_CANCELLED},
                 {"Filled", UMI_IBKR_ORDER_FILLED},
                 {"WarnState", UMI_IBKR_ORDER_WARNING}};
    UmiIbkrOrderPhase phase = UMI_IBKR_ORDER_UNRECOGNIZED;
    for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
        if (strcmp(names[i].text, text) == 0)
        {
            phase = names[i].phase;
            break;
        }
    *out = phase;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrReviewOrderObservation(const char *text, UmiDecimal filled, UmiDecimal remaining,
                                        UmiIbkrOrderObservation *out)
{
    if (out == NULL || filled.scale > 9U || remaining.scale > 9U || filled.coefficient < 0 ||
        remaining.coefficient < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrOrderObservation candidate = {0};
    UmiStatus status = UmiIbkrClassifyOrderPhase(text, &candidate.phase);
    if (status != UMI_STATUS_OK)
        return status;
    /* Copy before publishing so text may be borrowed from the previous output. */
    memcpy(candidate.providerStatus, text, strlen(text) + 1U);
    candidate.filled = filled;
    candidate.remaining = remaining;
    candidate.hasFills = filled.coefficient != 0;
    candidate.hasRemaining = remaining.coefficient != 0;
    candidate.cancellationPending = candidate.phase == UMI_IBKR_ORDER_PENDING_CANCEL;
    switch (candidate.phase)
    {
    case UMI_IBKR_ORDER_FILLED:
        candidate.conflictingQuantities = !candidate.hasFills || candidate.hasRemaining;
        if (!candidate.conflictingQuantities)
        {
            candidate.terminal = true;
            candidate.hasCanonicalStatus = true;
            candidate.canonicalStatus = UMI_ORDER_FILLED;
        }
        break;
    case UMI_IBKR_ORDER_CANCELLED:
        candidate.terminal = true;
        candidate.hasCanonicalStatus = true;
        candidate.canonicalStatus = UMI_ORDER_CANCELLED;
        break;
    case UMI_IBKR_ORDER_WORKING:
        /* A working message with no reported remainder needs another broker
         * observation. Do not promote it to Filled from quantity arithmetic. */
        candidate.conflictingQuantities = !candidate.hasRemaining;
        if (!candidate.conflictingQuantities)
        {
            candidate.hasCanonicalStatus = true;
            candidate.canonicalStatus = candidate.hasFills ? UMI_ORDER_PARTIALLY_FILLED : UMI_ORDER_ACCEPTED;
        }
        break;
    case UMI_IBKR_ORDER_PENDING_SUBMIT:
    case UMI_IBKR_ORDER_PRE_SUBMITTED:
        if (!candidate.hasFills && candidate.hasRemaining)
        {
            candidate.hasCanonicalStatus = true;
            candidate.canonicalStatus = UMI_ORDER_VALIDATED;
        }
        break;
    default:
        /* Pending, warning and unknown states retain their evidence without
         * inventing a local transition. The caller can display the raw status. */
        break;
    }
    *out = candidate;
    return UMI_STATUS_OK;
}
