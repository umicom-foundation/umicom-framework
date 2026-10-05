/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/quotes.h
 * PURPOSE: Observe bounded, read-only TWS quote subscriptions with explicit freshness and data type.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_QUOTES_H
#define UMICOM_BROKER_CONNECTIVITY_QUOTES_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_IBKR_QUOTE_CAPACITY 16U

/* Use the contract ID and exchange shown in TWS Contract Description. This
 * avoids guessing a security from a ticker shared by several asset classes.
 * Contract discovery, combinations and automatic rerouting are separate work. */
typedef struct UmiIbkrQuoteContract {
    uint32_t contractId;
    char exchange[64];
} UmiIbkrQuoteContract;

typedef enum UmiIbkrMarketDataType {
    UMI_IBKR_DATA_UNKNOWN = 0,
    UMI_IBKR_DATA_REALTIME = 1,
    UMI_IBKR_DATA_FROZEN = 2,
    UMI_IBKR_DATA_DELAYED = 3,
    UMI_IBKR_DATA_DELAYED_FROZEN = 4
} UmiIbkrMarketDataType;

/* Decimal text is kept exactly as received, not rounded into binary money.
 * The timestamp measures local receipt, not exchange time. Staleness describes
 * observation age, not proof that the exchange price changed. */
typedef struct UmiIbkrQuoteValue {
    char text[96];
    uint64_t receivedAtMilliseconds;
    UmiIbkrMarketDataType dataType;
    bool received, unavailable, stale;
} UmiIbkrQuoteValue;

typedef struct UmiIbkrQuoteSnapshot {
    uint32_t requestId;
    UmiIbkrQuoteContract contract;
    bool subscribed, failed;
    UmiIbkrMarketDataType dataType;
    uint64_t requestedAtMilliseconds;
    UmiIbkrQuoteValue bid, ask, last, bidSize, askSize, lastSize;
    int providerCode;
    char message[512];
} UmiIbkrQuoteSnapshot;

/* The connection must be READY. Requests ask for streaming subscribed data;
 * regulatory snapshots, which may carry a separate fee, are never requested.
 * Up to 16 subscriptions can coexist. A contract already subscribed returns
 * ALREADY_EXISTS. Failed subscriptions occupy their slot until cancelled.
 * Request IDs are never reused within a connection; late callbacks cannot
 * become observations for a newly selected contract. Output is untouched on
 * failure, and partial request bytes are never published to the socket queue. */
UmiStatus UmiIbkrQuoteSubscribe(UmiIbkrConnection *connection,
    const UmiIbkrQuoteContract *contract, uint64_t nowMilliseconds,
    uint32_t *outRequestId);
/* Cancels market data only, never an order. On queue failure the subscription
 * stays active so the owner can retry or disconnect. */
UmiStatus UmiIbkrQuoteCancel(UmiIbkrConnection *connection, uint32_t requestId);
/* Copy owned evidence at a supplied monotonic time. Each value is stale when
 * cancelled, disconnected, failed, unavailable, of unknown/mismatched data
 * type, or older than maximumAgeMilliseconds (which must be nonzero).
 * Delayed/frozen types remain explicit even when their receipt is recent.
 * A cancelled snapshot is retained until its slot is reused. */
UmiStatus UmiIbkrQuoteCopy(const UmiIbkrConnection *connection, uint32_t requestId,
    uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
    UmiIbkrQuoteSnapshot *out);
const char *UmiIbkrMarketDataTypeName(UmiIbkrMarketDataType type);
#ifdef __cplusplus
}
#endif
#endif
