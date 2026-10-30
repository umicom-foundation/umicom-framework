/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/time_sales.h
 *
 * PURPOSE:
 *   Own bounded tick-by-tick observations for Time & Sales consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_TIME_SALES_H
#define UMICOM_BROKER_CONNECTIVITY_TIME_SALES_H
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/broker_connectivity/order_recovery.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IBKR_TIME_SALES_STREAM_LIMIT 4U
#define UMI_IBKR_TIME_SALES_EVENT_LIMIT 512U
#define UMI_IBKR_TIME_SALES_INTERVAL_MS UINT64_C(15000)

typedef enum UmiIbkrTimeSalesKind {
    UMI_IBKR_TICKS_LAST = 1,
    UMI_IBKR_TICKS_ALL_LAST = 2,
    UMI_IBKR_TICKS_BID_ASK = 3,
    UMI_IBKR_TICKS_MIDPOINT = 4
} UmiIbkrTimeSalesKind;

typedef struct UmiIbkrTimeSalesQuery {
    UmiIbkrQuoteContract contract;
    UmiIbkrTimeSalesKind kind;
    bool ignoreSize; /* Valid only for bid/ask subscriptions. */
} UmiIbkrTimeSalesQuery;

/* Prices and sizes use the shared lossless numeric representation. Read
 * reportedText when exact is false; never treat an unrepresentable value as
 * zero. A size retains provider units. Equal ticks are distinct events, since
 * the feed does not supply an execution identifier for deduplication.
 *
 * Last/AllLast use price and size; MidPoint uses price; BidAsk uses the four
 * bid/ask values. Flag interpretation depends on kind: Last bit 0 is past
 * limit and bit 1 unreported; BidAsk bit 0 is bid past low and bit 1 ask past
 * high. Zero-price trade markers and their flags remain intact for consumers. */
typedef struct UmiIbkrTimeSalesEvent {
    UmiIbkrTimeSalesKind kind;
    uint64_t sequence;
    uint64_t epochSeconds;
    uint64_t receivedAtMilliseconds;
    unsigned flags;
    UmiIbkrOrderNumber price, size;
    UmiIbkrOrderNumber bidPrice, askPrice, bidSize, askSize;
    char exchange[64];
    char specialConditions[256];
} UmiIbkrTimeSalesEvent;

typedef struct UmiIbkrTimeSalesSnapshot {
    uint32_t requestId;
    UmiIbkrTimeSalesQuery query;
    bool active, needsCancel, cancelled, failed, stale;
    uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
    uint64_t receivedEvents, droppedEvents;
    size_t count;
    int providerCode;
    char message[512];
} UmiIbkrTimeSalesSnapshot;

/* These functions share the connection's single-owner calling contract.
 * This service requests live ticks only (no historical prelude), on the
 * established legacy protocol range 151..176. Broker entitlements still apply.
 * A conservative 15-second gate covers ALL tick requests on this connection,
 * including retries after cancellation. It does not police other API clients.
 * Queue acceptance commits identity and storage; outputs stay intact on error. */
UmiStatus UmiIbkrTimeSalesQueryValidate(const UmiIbkrTimeSalesQuery *query);
UmiStatus UmiIbkrTimeSalesRequest(UmiIbkrConnection *connection,
    const UmiIbkrTimeSalesQuery *query, uint64_t nowMilliseconds,
    uint32_t *outRequestId);
UmiStatus UmiIbkrTimeSalesCancel(UmiIbkrConnection *connection,
    uint32_t requestId, uint64_t nowMilliseconds);
/* Copies own their data. Events remain in receipt order, not sorted by
 * exchange time. Full windows evict only their oldest event and count it.
 * Cancelled/failed rows survive until a later request reuses their slot. */
UmiStatus UmiIbkrTimeSalesCopy(const UmiIbkrConnection *connection,
    uint32_t requestId, uint64_t nowMilliseconds, uint64_t maximumAgeMilliseconds,
    UmiIbkrTimeSalesSnapshot *outSnapshot);
UmiStatus UmiIbkrTimeSalesEventCopy(const UmiIbkrConnection *connection,
    uint32_t requestId, size_t index, UmiIbkrTimeSalesEvent *outEvent);

#ifdef __cplusplus
}
#endif
#endif
