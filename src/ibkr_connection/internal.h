/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Read-only IBKR session: an explicit Paper/Live selection is intent, not proof
 * of the environment running in TWS. No order-submission API exists here.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IBKR_CONNECTION_INTERNAL_H
#define UMICOM_IBKR_CONNECTION_INTERNAL_H
#include "umicom/broker_connectivity/connection.h"
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/broker_connectivity/contract_details.h"
#include "umicom/broker_connectivity/market_rule.h"
#include "umicom/broker_connectivity/execution_observation.h"
#include "umicom/broker_connectivity/pnl.h"
#include "umicom/broker_connectivity/market_depth.h"
#include "umicom/broker_connectivity/symbol_search.h"
#include "order_private.h"
#include "completed_private.h"
#include "historical_private.h"
#include "realtime_private.h"
#include "discovery_private.h"
#include "scanner_catalog_private.h"
#define UMI_IBKR_FRAME_LIMIT 65536U
#define UMI_IBKR_TX_LIMIT 8192U
/* Private injectable I/O permits deterministic failure tests. BUSY means no
 * progress. OK with a zero-byte read means EOF, never an empty application frame.
 * Context remains caller-owned; Close is called exactly once after Open. */
typedef struct UmiIbkrIo {
    void *context;
    UmiStatus (*Open)(void *, uint16_t);
    UmiStatus (*Ready)(void *);
    UmiStatus (*Read)(void *, void *, size_t, size_t *);
    UmiStatus (*Write)(void *, const void *, size_t, size_t *);
    void (*Close)(void *);
} UmiIbkrIo;
struct UmiIbkrConnection {
    UmiIbkrConnectionOptions options;
    UmiIbkrConnectionSnapshot snapshot;
    UmiIbkrIo io;
    void (*DestroyIo)(void *);
    bool ioOpened, opened, accountsReceived, nextIdReceived, pingPending;
    uint64_t startedAt, lastNow, pingAt;
    unsigned char rx[UMI_IBKR_FRAME_LIMIT+4U], tx[UMI_IBKR_TX_LIMIT];
    size_t rxSize, txSize;
    UmiIbkrQuoteSnapshot quotes[UMI_IBKR_QUOTE_CAPACITY];
    /* Shared observation sequence; retain the established private member name. */
    uint32_t nextQuoteRequest;
    UmiIbkrContractDetailsSnapshot contractDetails;
    /* Rule IDs cannot be reissued: replies do not carry a query generation. */
    UmiIbkrExecutionSnapshot executions;
    UmiIbkrPnlSnapshot pnl[UMI_IBKR_PNL_CAPACITY];
    UmiIbkrSymbolSearchSnapshot symbolSearch;
    UmiIbkrDepthSnapshot depth[UMI_IBKR_DEPTH_STREAM_LIMIT];
    /* Retained broker order evidence shares this connection lifetime. */
    UmiIbkrOrderStore orders;
    /* Completed history owns separate rows because it has no API order ID. */
    UmiIbkrCompletedStore completed;
    /* The finite bar series is allocated only when requested and belongs to
     * this connection. Applications receive copies, never borrowed rows. */
    UmiIbkrHistoricalStore *history;
    uint64_t lastHistoryRequestAt;
    bool historyRequested;
    /* Allocate each rolling window on demand; copied public records never
     * borrow these buffers. All bar requests share a conservative pacing gate. */
    UmiIbkrRealtimeStore *realtime[UMI_IBKR_REALTIME_STREAM_LIMIT];
    uint64_t lastBarRequestAt;
    bool barRequested;
    /* Discovery allocates bounded storage only when a user requests it. Results
     * are copied to consumers; no application borrows a mutable broker row. */
    UmiIbkrScannerStore *scanners[UMI_IBKR_SCANNER_LIMIT];
    UmiIbkrOptionChainStore *optionChains;
    uint64_t lastScannerRequestAt, lastOptionChainRequestAt;
    bool scannerRequested, optionChainRequested;
    /* Catalogue documents alone may need an envelope larger than market data.
     * These buffers are connection-owned and released on close or completion. */
    UmiIbkrScannerCatalogStore scannerCatalog;
    unsigned char *catalogFrame;
    size_t catalogFrameSize, catalogFrameCapacity;
    size_t marketRuleCount;
    UmiIbkrMarketRuleSnapshot marketRules[UMI_IBKR_MARKET_RULE_LIMIT];
};
UmiStatus UmiIbkrConnectionCreateWithIo(const UmiIbkrConnectionOptions *, const UmiIbkrIo *, UmiIbkrConnection **);
UmiStatus UmiIbkrNativeIo(UmiIbkrIo *outIo, void (**outDestroy)(void *));
UmiStatus UmiIbkrProcessFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrQueueFields(UmiIbkrConnection *, const char *const *, size_t);
/* Private wire callbacks share the connection's bounded request ownership. */
UmiStatus UmiIbkrQuoteFrame(UmiIbkrConnection *, uint64_t, char **, size_t, uint64_t);
bool UmiIbkrQuoteProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
UmiStatus UmiIbkrContractDetailsFrame(UmiIbkrConnection *, uint64_t,
    const unsigned char *, size_t, uint64_t);
bool UmiIbkrContractDetailsProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
UmiStatus UmiIbkrMarketRuleFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
bool UmiIbkrText(const char *text, size_t capacity, bool allowEmpty);
bool UmiIbkrUnsigned(const char *, uint64_t *);
bool UmiIbkrDecimalText(const char *);
UmiStatus UmiIbkrExecutionsFrame(UmiIbkrConnection *, uint64_t, const unsigned char *, size_t, uint64_t);
bool UmiIbkrExecutionsProviderMessage(UmiIbkrConnection *,const char *,int,const char *);
bool UmiIbkrExecutionsAccepting(const UmiIbkrConnection *,uint32_t,uint64_t);
UmiStatus UmiIbkrCommissionFrame(UmiIbkrConnection *,const unsigned char *,size_t,uint64_t);
UmiStatus UmiIbkrPnlFrame(UmiIbkrConnection *,uint64_t,const unsigned char *,size_t,uint64_t);
bool UmiIbkrPnlProviderMessage(UmiIbkrConnection *,const char *,int,const char *);
UmiStatus UmiIbkrSymbolSearchFrame(UmiIbkrConnection *,const unsigned char *,size_t,uint64_t);
bool UmiIbkrSymbolSearchProviderMessage(UmiIbkrConnection *,const char *,int,const char *);
bool UmiIbkrSymbolSearchAccepting(const UmiIbkrConnection *,uint32_t,uint64_t);
UmiStatus UmiIbkrDepthFrame(UmiIbkrConnection *,uint64_t,const unsigned char *,size_t,uint64_t);
bool UmiIbkrDepthProviderMessage(UmiIbkrConnection *,const char *,int,const char *);
#endif
