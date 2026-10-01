/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/connection.h
 * PURPOSE: Own read-only IBKR connection state without an order-submission interface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Read-only IBKR session: an explicit Paper/Live selection is intent, not proof
 * of the environment running in TWS. No order-submission API exists here.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IBKR_CONNECTION_H
#define UMICOM_IBKR_CONNECTION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/broker_connectivity/ibkr_adapter.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_IBKR_ACCOUNT_LIMIT 32U
#define UMI_IBKR_VALUE_LIMIT 64U
#define UMI_IBKR_POSITION_LIMIT 64U
#define UMI_IBKR_TEXT_CAPACITY 128U

typedef enum UmiIbkrProgram { UMI_IBKR_TWS = 0, UMI_IBKR_GATEWAY = 1 } UmiIbkrProgram;
typedef enum UmiIbkrConnectionState {
    UMI_IBKR_IDLE = 0, UMI_IBKR_CONNECTING, UMI_IBKR_HANDSHAKE,
    UMI_IBKR_WAITING, UMI_IBKR_READY, UMI_IBKR_DISCONNECTED, UMI_IBKR_FAILED
} UmiIbkrConnectionState;

typedef struct UmiIbkrConnectionOptions {
    UmiIbkrAdapterConfig adapter;
    UmiTradingEnvironment environment;
    UmiIbkrProgram program;
    uint32_t timeoutMilliseconds;
    bool acknowledgeLive;
} UmiIbkrConnectionOptions;

/* Observations, not booked balances or canonical financial positions. Values
 * remain provider text so decimals, unset values and contract details are not
 * silently rounded or reinterpreted by a display-only adapter. */
typedef struct UmiIbkrAccountValue {
    char tag[64]; char value[UMI_IBKR_TEXT_CAPACITY]; char currency[16];
} UmiIbkrAccountValue;
typedef struct UmiIbkrPositionObservation {
    char contractId[24]; char symbol[96]; char securityType[24];
    char expiry[64]; char strike[64]; char right[16]; char multiplier[64];
    char exchange[64]; char currency[16]; char localSymbol[96]; char tradingClass[64];
    char quantity[96]; char averageCost[96];
} UmiIbkrPositionObservation;
typedef struct UmiIbkrConnectionSnapshot {
    UmiIbkrConnectionState state;
    UmiTradingEnvironment requestedEnvironment;
    bool readOnly;
    bool environmentAttested; /* Always false: the framed API does not attest it. */
    int protocolVersion;
    bool clockReceived;
    uint64_t serverEpochSeconds;
    uint64_t clockReceivedAtMilliseconds;
    char accounts[UMI_IBKR_ACCOUNT_LIMIT][64];
    size_t accountCount;
    char selectedAccount[64];
    /* stale describes connection failure/closure. Complete responses remain
     * historical observations even while the transport is healthy; inspect
     * the completion times rather than treating this flag as data freshness. */
    bool requestIssued, summaryComplete, positionsComplete, stale;
    uint64_t requestedAtMilliseconds, summaryAtMilliseconds, positionsAtMilliseconds;
    UmiIbkrAccountValue values[UMI_IBKR_VALUE_LIMIT];
    size_t valueCount;
    UmiIbkrPositionObservation positions[UMI_IBKR_POSITION_LIMIT];
    size_t positionCount;
    uint64_t framesReceived, ignoredFrames;
    UmiStatus lastStatus;
    int providerCode;
    char message[512];
} UmiIbkrConnectionSnapshot;
typedef struct UmiIbkrConnection UmiIbkrConnection;

/* No network operation. New profiles are Paper, read-only and loopback-only. */
UmiIbkrConnectionOptions UmiIbkrConnectionOptionsDefault(void);
uint16_t UmiIbkrDefaultPort(UmiIbkrProgram program, UmiTradingEnvironment environment);
UmiStatus UmiIbkrConnectionValidate(const UmiIbkrConnectionOptions *options);
UmiStatus UmiIbkrConnectionCreate(const UmiIbkrConnectionOptions *options, UmiIbkrConnection **outConnection);
/* Single-owner contract: serialise all calls, including Copy and Destroy.
 * Open is explicit. Each instance opens at most once; reconnect creates a new
 * instance, eliminating stale callbacks and cross-account stream reuse. */
UmiStatus UmiIbkrConnectionOpen(UmiIbkrConnection *connection, uint64_t nowMilliseconds);
/* Nonblocking; bounded to 64 frames and 64 KiB of input per call. */
UmiStatus UmiIbkrConnectionPump(UmiIbkrConnection *connection, uint64_t nowMilliseconds);
/* One selected-account snapshot per connection, only after the API is ready.
 * The account must exactly match the list received on this same connection. */
UmiStatus UmiIbkrConnectionReadAccount(UmiIbkrConnection *connection, const char *account, uint64_t nowMilliseconds);
UmiStatus UmiIbkrConnectionCopy(const UmiIbkrConnection *connection, UmiIbkrConnectionSnapshot *outSnapshot);
void UmiIbkrConnectionClose(UmiIbkrConnection *connection);
void UmiIbkrConnectionDestroy(UmiIbkrConnection *connection);
uint64_t UmiIbkrMonotonicMilliseconds(void);
const char *UmiIbkrConnectionStateName(UmiIbkrConnectionState state);
#ifdef __cplusplus
}
#endif
#endif
