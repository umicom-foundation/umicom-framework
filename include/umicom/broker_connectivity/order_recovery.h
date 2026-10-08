/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/order_recovery.h
 * PURPOSE: Recover broker order summaries and status observations without transmitting orders.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_ORDER_RECOVERY_H
#define UMICOM_BROKER_CONNECTIVITY_ORDER_RECOVERY_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/finance/decimal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_IBKR_ORDER_LIMIT 64U
    /* Scope describes the request, not ownership or permission to modify an order.
 * All-clients recovery is a one-time snapshot. Later notifications depend on
 * the client and Master Client settings in TWS; they are not promised here. */
    typedef enum UmiIbkrOrderScope
    {
        UMI_IBKR_ORDERS_THIS_CLIENT = 1,
        UMI_IBKR_ORDERS_ALL_CLIENTS = 2
    } UmiIbkrOrderScope;
    typedef struct UmiIbkrOrderNumber
    {
        char reportedText[96];
        UmiDecimal value;
        bool exact; /* Inspect before arithmetic; an unset value is never zero. */
    } UmiIbkrOrderNumber;
    typedef struct UmiIbkrOrderKey
    {
        int32_t clientId, orderId; /* Bound manual orders can have negative IDs. */
    } UmiIbkrOrderKey;
    typedef struct UmiIbkrOpenOrderSummary
    {
        uint32_t contractId;
        char symbol[64], securityType[16], expiry[32], right[8], multiplier[32];
        char exchange[64], currency[16], localSymbol[64], tradingClass[64];
        char action[16], orderType[32], timeInForce[16], ocaGroup[128], account[64];
        char openClose[8], orderReference[256];
        int32_t origin;
        UmiIbkrOrderNumber strike, totalQuantity, limitPrice, auxiliaryPrice;
        size_t wireFieldCount;
        /* This structure interprets the stable identity/contract/order prefix.
     * Advanced routing, conditions and OrderState remain raw wire fields.
     * A captured payload is evidence, not a fully validated order ticket. */
    } UmiIbkrOpenOrderSummary;
    typedef struct UmiIbkrOrderStatusObservation
    {
        char status[64], whyHeld[256];
        int32_t parentId;
        UmiIbkrOrderNumber filled, remaining, averageFillPrice, lastFillPrice, marketCapPrice;
    } UmiIbkrOrderStatusObservation;
    typedef struct UmiIbkrRecoveredOrder
    {
        UmiIbkrOrderKey key;
        uint64_t permanentId;
        UmiIbkrOpenOrderSummary open;
        UmiIbkrOrderStatusObservation status;
        bool hasOpenOrder, hasStatus, openStale, statusStale;
        uint64_t openReceivedAtMilliseconds, statusReceivedAtMilliseconds;
        uint64_t revision, duplicateStatuses;
    } UmiIbkrRecoveredOrder;
    typedef struct UmiIbkrOrderRecoverySnapshot
    {
        UmiIbkrOrderScope scope;
        size_t count;
        bool requested, pending, complete, failed, stale;
        uint64_t requestedAtMilliseconds, completedAtMilliseconds, revision;
        char message[256];
    } UmiIbkrOrderRecoverySnapshot;
    typedef struct UmiIbkrOrderIdentitySnapshot
    {
        uint32_t brokerNextOrderId, highestObservedOrderId, conservativeNextOrderId;
        bool brokerIdReceived, observedIdReceived, exhausted, stale;
        /* The conservative value exceeds nonnegative IDs decoded in open/status
     * reports and accepted execution captures. It is not complete visibility
     * into all accounts or callback types. It is a hint, not a reservation, persistent allocator or
     * permission to submit. A trading owner must reconcile again before use. */
    } UmiIbkrOrderIdentitySnapshot;
    /* Recover once per connection. The protocol's end marker has no request ID:
 * reconnect for a new snapshot rather than confuse a late end with a retry.
 * Queue failures do not consume this opportunity. Client ID zero is refused
 * by the connection owner, so this request does not bind manual TWS orders. */
    UmiStatus UmiIbkrOrdersRequest(UmiIbkrConnection *connection, UmiIbkrOrderScope scope,
                                   uint64_t nowMilliseconds);
    /* Copy operations preserve the caller's output on invalid input. They do not
 * pump I/O. Use the same owner thread as the connection and keep pumping it.
 * Timeout retires the capture; a late end marker cannot make it complete. */
    UmiStatus UmiIbkrOrdersCopy(const UmiIbkrConnection *connection, uint64_t nowMilliseconds,
                                uint64_t maximumAgeMilliseconds, UmiIbkrOrderRecoverySnapshot *out);
    UmiStatus UmiIbkrOrderCopy(const UmiIbkrConnection *connection, size_t index, uint64_t nowMilliseconds,
                               uint64_t maximumAgeMilliseconds, UmiIbkrRecoveredOrder *out);
    UmiStatus UmiIbkrOrderIdentityCopy(const UmiIbkrConnection *connection,
                                       UmiIbkrOrderIdentitySnapshot *out);
    /* Inspect a captured legacy openOrder field by its zero-based wire index.
 * Field zero is the message ID. Names and tail layouts depend on the negotiated
 * protocol; raw fields must never be replayed as an outgoing order.
 * required includes the NUL. A short buffer returns CAPACITY_EXCEEDED, writes
 * required only, and leaves the buffer untouched. No pointer into the session
 * escapes. Treat account/reference text as private when exporting it. */
    UmiStatus UmiIbkrOpenOrderFieldCopy(const UmiIbkrConnection *connection, size_t orderIndex,
                                        size_t fieldIndex, char *buffer, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
