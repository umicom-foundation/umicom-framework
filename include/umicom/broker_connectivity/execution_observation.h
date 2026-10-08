/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/execution_observation.h
 * PURPOSE: Capture broker execution reports without changing orders or account state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_EXECUTION_OBSERVATION_H
#define UMICOM_BROKER_CONNECTIVITY_EXECUTION_OBSERVATION_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/finance/decimal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_EXECUTION_OBSERVATION_LIMIT 256U
    /* These are copied observations, not order-management handles. Quantity and
 * price use exact decimals; provider times remain labelled text because their
 * time zone cannot be inferred safely from the workstation's local clock. */
    typedef struct UmiIbkrExecutionObservation
    {
        char executionId[128], account[64], symbol[64], securityType[16];
        char currency[16], side[8], exchange[64], time[64], orderReference[128];
        uint32_t contractId;
        int32_t orderId, clientId;
        uint64_t permanentOrderId;
        UmiDecimal quantity, price, cumulativeQuantity, averagePrice;
    } UmiIbkrExecutionObservation;
    /* Fees arrive separately and have no request ID. Only reports matching an
 * already captured execution are retained. Missing reports stay unavailable,
 * not zero. Conflicting amounts/currencies retain first and latest text for
 * review and disable aggregation; no exchange rate is invented. */
    typedef struct UmiIbkrExecutionFee
    {
        bool received, exact, conflicting;
        char firstReportedAmount[96], latestReportedAmount[96];
        char firstCurrency[16], latestCurrency[16];
        UmiDecimal amount;
        uint64_t receivedAtMilliseconds;
    } UmiIbkrExecutionFee;
    typedef struct UmiIbkrExecutionSnapshot
    {
        uint32_t requestId;
        char account[64], message[256];
        uint64_t requestedAtMilliseconds, revision, duplicateCount;
        size_t count;
        bool complete, failed, stale;
        UmiIbkrExecutionObservation rows[UMI_IBKR_EXECUTION_OBSERVATION_LIMIT];
        UmiIbkrExecutionFee fees[UMI_IBKR_EXECUTION_OBSERVATION_LIMIT];
    } UmiIbkrExecutionSnapshot;
    /* The selected account must be one advertised by this connection. The request
 * uses the provider's available recent execution window and default client
 * filter. It does not promise a complete account history or change API binding.
 * One capture is retained at a time. A new accepted request replaces the old
 * capture; callers may copy it first. No request is retried automatically. */
    UmiStatus UmiIbkrExecutionsRequest(UmiIbkrConnection *connection, const char *account,
                                       uint64_t nowMilliseconds, uint32_t *outRequestId);
    UmiStatus UmiIbkrExecutionsCopy(const UmiIbkrConnection *connection, uint32_t requestId,
                                    uint64_t nowMilliseconds, UmiIbkrExecutionSnapshot *out);
    /* Retire only this local read request. There is no cancel-order operation. */
    UmiStatus UmiIbkrExecutionsAbandon(UmiIbkrConnection *connection, uint32_t requestId);
    /* Compare business fields, excluding padding, so replay cannot add quantity
 * twice. Both observations must be valid; no caller storage is retained. */
    bool UmiIbkrExecutionObservationValid(const UmiIbkrExecutionObservation *value);
    bool UmiIbkrExecutionObservationEqual(const UmiIbkrExecutionObservation *left,
                                          const UmiIbkrExecutionObservation *right);
    /* A stock-only review groups by account, positive permanent order ID, conId
 * and side. It compares observed quantity with the caller's intended quantity;
 * it cannot certify an order is finished, or that FOK/AON was enforced.
 * Corrections and combo-style execution IDs are retained but block aggregation:
 * choosing the newest correction from arrival order would be unsafe. */
    typedef struct UmiIbkrExecutionQuantityReview
    {
        UmiDecimal observedQuantity;
        size_t executionCount;
        bool belowRequested, equalsRequested, exceedsRequested;
        bool correctionNeedsReview, unsupportedExecutionShape;
    } UmiIbkrExecutionQuantityReview;
    UmiStatus UmiIbkrExecutionsReviewQuantity(const UmiIbkrExecutionSnapshot *capture,
                                              uint64_t permanentOrderId, uint32_t contractId,
                                              const char *side, UmiDecimal requestedQuantity,
                                              UmiIbkrExecutionQuantityReview *out);
    typedef struct UmiIbkrExecutionCommissionReview
    {
        UmiDecimal amount;
        char currency[16];
        size_t executionCount;
    } UmiIbkrExecutionCommissionReview;
    /* Sum only when the stock quantity review is unambiguous and every selected
 * execution has one consistent exact fee in the same currency. Missing fees
 * return UNAVAILABLE, conflicting reports INVALID_STATE, and mixed currencies
 * NOT_IMPLEMENTED. Failure leaves out unchanged. This is a captured report,
 * not a fee estimate or a guarantee that later adjustments cannot occur. */
    UmiStatus UmiIbkrExecutionsReviewCommission(const UmiIbkrExecutionSnapshot *capture,
                                                uint64_t permanentOrderId, uint32_t contractId,
                                                const char *side, UmiIbkrExecutionCommissionReview *out);
#ifdef __cplusplus
}
#endif
#endif
