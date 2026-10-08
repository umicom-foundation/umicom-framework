/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_review.c
 * PURPOSE: Compare completed-order evidence with separately captured stock fills and fees.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrCompletedOrderReviewExecutions(const UmiIbkrConnection *c, size_t index, uint32_t request,
                                                uint64_t now, uint64_t maxAge,
                                                UmiIbkrCompletedExecutionReview *out)
{
    if (c == NULL || out == NULL || request == 0U || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrCompletedOrdersSnapshot capture;
    UmiIbkrCompletedOrder row;
    UmiStatus status = UmiIbkrCompletedOrdersCopy(c, now, maxAge, &capture);
    if (status != UMI_STATUS_OK)
        return status;
    if (!capture.complete || capture.failed || capture.stale)
        return UMI_STATUS_UNAVAILABLE;
    status = UmiIbkrCompletedOrderCopy(c, index, now, maxAge, &row);
    if (status != UMI_STATUS_OK)
        return status;
    if (row.stale)
        return UMI_STATUS_UNAVAILABLE;
    /* The broker's final filled quantity is the comparison target, not the
     * original order quantity. A partially filled cancellation can therefore
     * agree with executions without being presented as a full fill. Zero or
     * inexact quantities cannot establish a positive execution comparison. */
    if (!row.filledQuantity.exact || row.filledQuantity.value.coefficient <= 0)
        return UMI_STATUS_UNAVAILABLE;
    if (strcmp(row.order.securityType, "STK") != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *side = strcmp(row.order.action, "BUY") == 0    ? "BOT"
                       : strcmp(row.order.action, "SELL") == 0 ? "SLD"
                                                               : NULL;
    if (side == NULL)
        return UMI_STATUS_NOT_IMPLEMENTED;
    /* Execution captures are large. Allocate a private copy so review does not
     * add a large automatic object to small native thread stacks. */
    UmiIbkrExecutionSnapshot *executions = malloc(sizeof *executions);
    if (executions == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiIbkrExecutionsCopy(c, request, now, executions);
    if (status == UMI_STATUS_OK && (!executions->complete || executions->failed || executions->stale ||
                                    now - executions->requestedAtMilliseconds > maxAge))
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK && strcmp(row.order.account, executions->account) != 0)
        status = UMI_STATUS_PERMISSION_DENIED;
    /* A permanent ID must not silently collect another contract or side.
     * Future multi-leg review needs its own explicit grouping rules. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < executions->count; ++i)
    {
        const UmiIbkrExecutionObservation *fill = &executions->rows[i];
        if (fill->permanentOrderId == row.permanentId &&
            (fill->contractId != row.order.contractId || strcmp(fill->side, side) != 0 ||
             strcmp(fill->currency, row.order.currency) != 0 ||
             strcmp(fill->securityType, row.order.securityType) != 0))
            status = UMI_STATUS_INVALID_STATE;
    }
    UmiIbkrCompletedExecutionReview result = {0};
    if (status == UMI_STATUS_OK)
        status = UmiIbkrExecutionsReviewQuantity(executions, row.permanentId, row.order.contractId, side,
                                                 row.filledQuantity.value, &result.quantity);
    if (status == UMI_STATUS_OK)
    {
        result.commissionStatus = UmiIbkrExecutionsReviewCommission(
            executions, row.permanentId, row.order.contractId, side, &result.commission);
        result.feeAvailable = result.commissionStatus == UMI_STATUS_OK;
        *out = result;
    }
    free(executions);
    return status;
}
