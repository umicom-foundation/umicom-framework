/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/completed_orders.h
 * PURPOSE: Recover completed broker orders as bounded, separately qualified evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_COMPLETED_ORDERS_H
#define UMICOM_BROKER_CONNECTIVITY_COMPLETED_ORDERS_H
#include "umicom/broker_connectivity/order_recovery.h"
#include "umicom/broker_connectivity/execution_observation.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_COMPLETED_ORDER_LIMIT 64U
    typedef struct UmiIbkrCompletedOrder
    {
        UmiIbkrOpenOrderSummary order;
        uint64_t permanentId, parentPermanentId;
        char status[64], completedStatus[256], completedTime[96], autoCancelDate[64], modelCode[64];
        UmiIbkrOrderNumber filledQuantity, cashQuantity, minimumQuantity;
        bool outsideRegularHours, hidden, sweepToFill, allOrNone, stale;
        size_t comboLegCount, conditionCount;
        uint64_t receivedAtMilliseconds, duplicateCount;
        /* Completed callbacks do not carry a client/API order binding. Never
     * fabricate one from the permanent ID. Provider time remains labelled text;
     * the connection cannot infer its time zone from the local clock. */
    } UmiIbkrCompletedOrder;
    typedef struct UmiIbkrCompletedOrdersSnapshot
    {
        size_t count;
        bool requested, apiOnly, pending, complete, failed, stale;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds;
        char message[256];
    } UmiIbkrCompletedOrdersSnapshot;
    /* apiOnly filters the provider request, not a particular account or API client.
 * The response is the broker's available completed-order window, not an audit
 * of all trading history. One request per connection prevents uncorrelated late
 * end markers from completing a retry. Queue failure does not consume it. */
    UmiStatus UmiIbkrCompletedOrdersRequest(UmiIbkrConnection *connection, bool apiOnly,
                                            uint64_t nowMilliseconds);
    UmiStatus UmiIbkrCompletedOrdersCopy(const UmiIbkrConnection *connection, uint64_t nowMilliseconds,
                                         uint64_t maximumAgeMilliseconds,
                                         UmiIbkrCompletedOrdersSnapshot *out);
    UmiStatus UmiIbkrCompletedOrderCopy(const UmiIbkrConnection *connection, size_t index,
                                        uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
                                        UmiIbkrCompletedOrder *out);
    /* Raw retained fields preserve advanced attributes that are structurally decoded
 * but not projected as typed public properties. They are never outgoing tickets.
 * required includes NUL; short buffers are unchanged and report required size. */
    UmiStatus UmiIbkrCompletedOrderFieldCopy(const UmiIbkrConnection *connection, size_t orderIndex,
                                             size_t fieldIndex, char *buffer, size_t capacity,
                                             size_t *required);
    typedef struct UmiIbkrCompletedExecutionReview
    {
        UmiIbkrExecutionQuantityReview quantity;
        UmiIbkrExecutionCommissionReview commission;
        UmiStatus commissionStatus;
        bool feeAvailable;
        /* Quantity compares distinct captured executions with the completed
     * callback's filledQuantity, not with the original requested quantity.
     * Matching quantities do not certify complete history or FOK/AON support. */
    } UmiIbkrCompletedExecutionReview;
    /* Review only fresh complete captures of one account, permanent ID, ordinary
 * stock contract and side. Corrections/ambiguous shapes retain existing
 * execution-review refusal rules. Zero or inexact final filled quantities return
 * UNAVAILABLE. Execution age is measured from the capture request; completed row
 * and end-marker ages must also be within the caller limit.
 * The output is unchanged on failure.
 * Missing or inconsistent fees do not turn a successful quantity comparison
 * into a fee estimate: inspect commissionStatus and feeAvailable separately. */
    UmiStatus UmiIbkrCompletedOrderReviewExecutions(const UmiIbkrConnection *connection, size_t index,
                                                    uint32_t executionRequestId, uint64_t nowMilliseconds,
                                                    uint64_t maximumAgeMilliseconds,
                                                    UmiIbkrCompletedExecutionReview *out);
#ifdef __cplusplus
}
#endif
#endif
