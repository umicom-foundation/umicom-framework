/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/session.c
 * PURPOSE:
 *   Read-only IBKR session: an explicit Paper/Live selection is intent, not proof of the
 *   environment running in TWS. No order-submission API exists here.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Read-only IBKR session: an explicit Paper/Live selection is intent, not proof
 * of the environment running in TWS. No order-submission API exists here.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

UmiIbkrConnectionOptions UmiIbkrConnectionOptionsDefault(void)
{
    UmiIbkrConnectionOptions o = {0};
    umi_ibkr_adapter_config_init(&o.adapter);
    o.adapter.readOnly = 1; o.adapter.clientId = 35;
    o.environment = UMI_TRADING_PAPER; o.program = UMI_IBKR_TWS;
    o.timeoutMilliseconds = 10000U;
    return o;
}
uint16_t UmiIbkrDefaultPort(UmiIbkrProgram program, UmiTradingEnvironment environment)
{
    if (environment != UMI_TRADING_PAPER && environment != UMI_TRADING_LIVE) return 0;
    if (program == UMI_IBKR_TWS) return environment == UMI_TRADING_PAPER ? 7497U : 7496U;
    if (program == UMI_IBKR_GATEWAY) return environment == UMI_TRADING_PAPER ? 4002U : 4001U;
    return 0;
}
UmiStatus UmiIbkrConnectionValidate(const UmiIbkrConnectionOptions *o)
{
    if (o == NULL || umi_ibkr_adapter_config_validate(&o->adapter) != UMI_STATUS_OK ||
        UmiIbkrDefaultPort(o->program, o->environment) == 0 ||
        o->adapter.clientId <= 0 || o->timeoutMilliseconds < 100U ||
        o->timeoutMilliseconds > 60000U || o->adapter.account[0] != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Refuse DNS, remote transports and client ID zero's special order binding.
     * Port is editable, so it is never used as evidence of the account mode. */
    if (strcmp(o->adapter.host, "127.0.0.1") != 0 || o->adapter.readOnly != 1)
        return UMI_STATUS_PERMISSION_DENIED;
    if (o->adapter.paperOnly != (o->environment == UMI_TRADING_PAPER ? 1 : 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (o->environment == UMI_TRADING_LIVE && !o->acknowledgeLive)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}
const char *UmiIbkrConnectionStateName(UmiIbkrConnectionState state)
{
    switch (state) {
    case UMI_IBKR_IDLE: return "Not connected";
    case UMI_IBKR_CONNECTING: return "Opening local socket";
    case UMI_IBKR_HANDSHAKE: return "Negotiating API protocol";
    case UMI_IBKR_WAITING: return "Waiting for accounts, API readiness and clock";
    case UMI_IBKR_READY: return "Read-only API ready";
    case UMI_IBKR_DISCONNECTED: return "Disconnected; observations retained as stale";
    case UMI_IBKR_FAILED: return "Connection failed; observations are stale";
    default: return "Invalid state";
    }
}
static void CloseIo(UmiIbkrConnection *c)
{
    UmiIbkrHistoricalConnectionClosed(c);
    UmiIbkrRealtimeConnectionClosed(c);
    UmiIbkrDiscoveryClosed(c);
    UmiIbkrScannerCatalogClosed(c);
    if (c->ioOpened) { c->io.Close(c->io.context); c->ioOpened = false; }
}
static UmiStatus Fail(UmiIbkrConnection *c, UmiStatus status, const char *message)
{
    CloseIo(c); c->snapshot.state = UMI_IBKR_FAILED;
    c->snapshot.stale = true; c->snapshot.lastStatus = status;
    c->txSize = 0; c->rxSize = 0;
    if (message != NULL) (void)snprintf(c->snapshot.message, sizeof c->snapshot.message, "%s", message);
    return status;
}
UmiStatus UmiIbkrConnectionCreateWithIo(const UmiIbkrConnectionOptions *o, const UmiIbkrIo *io, UmiIbkrConnection **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiIbkrConnectionValidate(o);
    if (status != UMI_STATUS_OK) return status;
    if (!io || !io->Open || !io->Ready || !io->Read || !io->Write || !io->Close)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrConnection *c = calloc(1, sizeof *c);
    if (!c) return UMI_STATUS_OUT_OF_MEMORY;
    c->options = *o; c->io = *io;
    c->snapshot.requestedEnvironment = o->environment;
    c->snapshot.readOnly = true; c->snapshot.stale = true;
    (void)snprintf(c->snapshot.message, sizeof c->snapshot.message,
        "No connection has been opened. Paper/Live is requested, not attested.");
    *out = c; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionCreate(const UmiIbkrConnectionOptions *o, UmiIbkrConnection **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiIbkrConnectionValidate(o);
    if (status != UMI_STATUS_OK) return status;
    UmiIbkrIo io = {0}; void (*destroy)(void *) = NULL;
    status = UmiIbkrNativeIo(&io, &destroy);
    if (status != UMI_STATUS_OK) return status;
    status = UmiIbkrConnectionCreateWithIo(o, &io, out);
    if (status != UMI_STATUS_OK) { destroy(io.context); return status; }
    (*out)->DestroyIo = destroy; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionOpen(UmiIbkrConnection *c, uint64_t now)
{
    if (!c) return UMI_STATUS_INVALID_ARGUMENT;
    if (c->opened) return UMI_STATUS_INVALID_STATE;
    c->opened = true; c->lastNow = now; c->startedAt = now;
    c->ioOpened = true; c->snapshot.state = UMI_IBKR_CONNECTING;
    UmiStatus status = c->io.Open(c->io.context, c->options.adapter.port);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_BUSY)
        return Fail(c, status, "Cannot open the selected local TWS/Gateway endpoint.");
    return UMI_STATUS_OK;
}
/* Only these audited read-only messages can enter the transmit buffer. Neither
 * order placement, cancellation, option exercise nor configuration mutation is
 * represented. StartApi is private to the handshake. */
UmiStatus UmiIbkrQueueFields(UmiIbkrConnection *c, const char *const *fields, size_t count)
{
    /* Quote subscriptions add request/cancel market data and data-type selection
     * to this read-only allowlist. Message 2 cancels market data; order cancel
     * (message 4) and order placement (message 3) remain forbidden. The previous
     * account-only allowlist is retained for review. */
#if 0
    if (!c || !fields || count < 2 || count > 6) return UMI_STATUS_INVALID_ARGUMENT;
    const char *id = fields[0];
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Historical requests contain 23 fields. The prior small-message bound is
     * retained for review; the byte limit and explicit message gate still apply. */
#if 0
    if (!c || !fields || count < 2U || count > 20U) return UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (!c || !fields || count < 2U || count > 32U) return UMI_STATUS_INVALID_ARGUMENT;
    const char *id = fields[0];
    /* Contract metadata adds read-only messages 9 and 91. Retain the earlier allowlist
     * for review; order placement and cancellation remain forbidden. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Recent execution capture adds read-only message 7. Preserve the previous
     * allowlist for comparison; placing or cancelling an order is not enabled. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Keep the preceding allowlist for review. Contract search and P&L
     * subscribe/cancel messages extend observation only; order commands remain
     * excluded from this connection owner. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Depth adds request 10 and cancel 11 to the observation transport.
     * The former gate remains for review; neither message changes an order. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Preserve the earlier observation gate for review. Open-order requests
     * add only 5 and 16; client zero, auto-binding and all order mutations stay
     * unavailable. The native request owner validates the recovery scope. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Completed history adds read request 99. Preserve the prior gate for
     * review; this extension grants no order mutation or automatic binding. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Historical data adds request 20 and cancellation 25. Retain the preceding
     * gate for review. These messages cannot place, bind or cancel an order. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16") && strcmp(id,"99")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Retain the finite-capture gate for review. Streaming subscribe/cancel
     * extends market observation only; order mutation messages remain excluded. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16") && strcmp(id,"99") && strcmp(id,"20") && strcmp(id,"25")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Scanner subscribe/cancel and option-definition discovery extend the
     * read-only gate. Retain its predecessor for review; order mutations and
     * broker configuration changes remain separate responsibilities. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16") && strcmp(id,"99") && strcmp(id,"20") && strcmp(id,"25") && strcmp(id,"50") && strcmp(id,"51")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    /* Catalogue discovery adds read-only request 24. Retain the previous gate
     * so the transport's authority remains straightforward to review. */
#if 0
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16") && strcmp(id,"99") && strcmp(id,"20") && strcmp(id,"25") && strcmp(id,"50") && strcmp(id,"51") && strcmp(id,"22") && strcmp(id,"23") && strcmp(id,"78")))
        return UMI_STATUS_PERMISSION_DENIED;
#endif
    if (!id || (strcmp(id,"71") && strcmp(id,"49") && strcmp(id,"62") &&
                strcmp(id,"63") && strcmp(id,"61") && strcmp(id,"64") &&
                strcmp(id,"1") && strcmp(id,"2") && strcmp(id,"59") && strcmp(id,"9") && strcmp(id,"91") && strcmp(id,"7") &&
                strcmp(id,"81") && strcmp(id,"92") && strcmp(id,"93") && strcmp(id,"94") && strcmp(id,"95") && strcmp(id,"10") && strcmp(id,"11") && strcmp(id,"5") && strcmp(id,"16") && strcmp(id,"99") && strcmp(id,"20") && strcmp(id,"25") && strcmp(id,"50") && strcmp(id,"51") && strcmp(id,"22") && strcmp(id,"23") && strcmp(id,"78") && strcmp(id,"24")))
        return UMI_STATUS_PERMISSION_DENIED;
    size_t length = 0;
    for (size_t i=0; i<count; ++i) {
        if (!UmiIbkrText(fields[i], 512U, true)) return UMI_STATUS_INVALID_ARGUMENT;
        size_t n = strlen(fields[i])+1U;
        if (n > UMI_IBKR_TX_LIMIT-4U-length) return UMI_STATUS_CAPACITY_EXCEEDED;
        length += n;
    }
    if (length+4U > UMI_IBKR_TX_LIMIT-c->txSize) return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *p=c->tx+c->txSize;
    p[0]=(unsigned char)(length>>24U);p[1]=(unsigned char)(length>>16U);
    p[2]=(unsigned char)(length>>8U);p[3]=(unsigned char)length;p+=4;
    for (size_t i=0;i<count;++i) { size_t n=strlen(fields[i])+1U;memcpy(p,fields[i],n);p+=n; }
    c->txSize += length+4U; return UMI_STATUS_OK;
}
static UmiStatus Ping(UmiIbkrConnection *c, uint64_t now)
{
    const char *fields[]={"49","1"};
    UmiStatus status=UmiIbkrQueueFields(c,fields,2);
    if (status==UMI_STATUS_OK) { c->pingPending=true; c->pingAt=now; }
    return status;
}
static UmiStatus Drain(UmiIbkrConnection *c, uint64_t now, size_t *budget)
{
    /* A large catalogue is accumulated over multiple pump calls. The usual
     * per-call byte budget remains unchanged, keeping the UI responsive. */
    if (c->catalogFrame) {
        if (c->catalogFrameSize >= 7U &&
            memcmp(c->catalogFrame + 4U, "19\0", 3U))
            return UMI_STATUS_PARSE_ERROR;
        if (c->catalogFrameSize < c->catalogFrameCapacity || !*budget)
            return UMI_STATUS_OK;
        if (c->snapshot.framesReceived == UINT64_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus status = UmiIbkrScannerCatalogFrame(c, c->catalogFrame + 4U,
            c->catalogFrameCapacity - 4U, now);
        if (status != UMI_STATUS_OK) return status;
        free(c->catalogFrame);
        c->catalogFrame = NULL;
        c->catalogFrameSize = c->catalogFrameCapacity = 0U;
        ++c->snapshot.framesReceived;
        --*budget;
    }
    while (*budget && c->rxSize>=4U) {
        size_t length=((size_t)c->rx[0]<<24U)|((size_t)c->rx[1]<<16U)|((size_t)c->rx[2]<<8U)|c->rx[3];
        /* Retain the former universal bound for review. Only the explicit
         * catalogue owner can admit a larger, separately bounded message. */
#if 0
        if (length==0 || length>UMI_IBKR_FRAME_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
        if (!length) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (length > UMI_IBKR_FRAME_LIMIT)
            return UmiIbkrScannerCatalogBeginFrame(c, length);
        if (c->rxSize<length+4U) return UMI_STATUS_OK;
        if (c->snapshot.framesReceived==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus status=UmiIbkrProcessFrame(c,c->rx+4U,length,now);
        if (status!=UMI_STATUS_OK) return status;
        ++c->snapshot.framesReceived; --*budget;
        c->rxSize-=length+4U;
        memmove(c->rx,c->rx+length+4U,c->rxSize);
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionPump(UmiIbkrConnection *c, uint64_t now)
{
    if (!c || now<c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->ioOpened || c->snapshot.state<UMI_IBKR_CONNECTING || c->snapshot.state>UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    c->lastNow=now;
    UmiIbkrOrdersExpire(c,now);
    UmiIbkrCompletedExpire(c,now);
    UmiIbkrHistoricalExpire(c,now);
    if (c->snapshot.state!=UMI_IBKR_READY && now-c->startedAt>=c->options.timeoutMilliseconds)
        return Fail(c,UMI_STATUS_TIMEOUT,"API handshake timed out. Check login, API settings, port and client ID.");
    if (c->snapshot.requestIssued && (!c->snapshot.summaryComplete || !c->snapshot.positionsComplete) &&
        now-c->snapshot.requestedAtMilliseconds>=c->options.timeoutMilliseconds)
        return Fail(c,UMI_STATUS_TIMEOUT,"The account response did not finish. Partial rows are not a complete snapshot.");
    if (c->pingPending && now-c->pingAt>=c->options.timeoutMilliseconds)
        return Fail(c,UMI_STATUS_TIMEOUT,"The API clock request timed out; reconnect explicitly.");
    if (c->snapshot.state==UMI_IBKR_CONNECTING) {
        UmiStatus status=c->io.Ready(c->io.context);
        if (status==UMI_STATUS_BUSY) return UMI_STATUS_OK;
        if (status!=UMI_STATUS_OK) return Fail(c,status,"Local socket connection was refused or failed.");
        /* Negotiate a bounded legacy field protocol: no protobuf decoding is
         * claimed. Unsupported server versions fail before any account query. */
        static const unsigned char hello[]={ 'A','P','I',0,0,0,0,9,'v','1','5','1','.','.','1','7','6' };
        memcpy(c->tx,hello,sizeof hello);c->txSize=sizeof hello;
        c->snapshot.state=UMI_IBKR_HANDSHAKE;
    }
    if (c->txSize) {
        size_t sent=0;UmiStatus status=c->io.Write(c->io.context,c->tx,c->txSize,&sent);
        if (sent>c->txSize || (status==UMI_STATUS_OK && sent==0) || (status==UMI_STATUS_BUSY && sent!=0)) return Fail(c,UMI_STATUS_IO_ERROR,"Invalid transport write progress.");
        if (status!=UMI_STATUS_OK && status!=UMI_STATUS_BUSY) return Fail(c,status,"Connection write failed; no automatic retry on a new connection.");
        if (status==UMI_STATUS_OK) { c->txSize-=sent;memmove(c->tx,c->tx+sent,c->txSize); }
    }
    size_t frames=64U, bytes=UMI_IBKR_FRAME_LIMIT;
    UmiStatus status=Drain(c,now,&frames);
    if (status!=UMI_STATUS_OK) return Fail(c,status,c->snapshot.message[0]?NULL:"Invalid API message.");
    while (frames && bytes) {
        /* The previous fixed receive destination remains below for review.
         * A promoted catalogue uses its own buffer; ordinary frames still use
         * the original storage and bounds. No borrowed pointer escapes Pump. */
#if 0
        size_t room=sizeof c->rx-c->rxSize;
#endif
        unsigned char *destination = c->catalogFrame ?
            c->catalogFrame + c->catalogFrameSize : c->rx + c->rxSize;
        size_t room = c->catalogFrame ?
            c->catalogFrameCapacity - c->catalogFrameSize : sizeof c->rx - c->rxSize;
        if (!room) return Fail(c,UMI_STATUS_CAPACITY_EXCEEDED,"Receive frame exceeds the configured bound.");
        if (room>bytes) room=bytes;
        if (room>4096U) room=4096U;
        /* Read into the selected owned envelope; preserve the fixed-buffer
         * call for comparison with the bounded catalogue path. */
#if 0
        size_t received=0;status=c->io.Read(c->io.context,c->rx+c->rxSize,room,&received);
#endif
        size_t received=0;status=c->io.Read(c->io.context,destination,room,&received);
        if (status==UMI_STATUS_BUSY) {
            if(received!=0) return Fail(c,UMI_STATUS_IO_ERROR,"Invalid transport read progress.");
            break;
        }
        if (status!=UMI_STATUS_OK || received==0 || received>room)
            return Fail(c,UMI_STATUS_IO_ERROR,"Connection closed or read failed; retained observations are stale.");
        /* Keep each receive size with its owning buffer. The earlier counter
         * update remains for review because it applies to ordinary frames. */
#if 0
        c->rxSize+=received;bytes-=received;
#endif
        if (c->catalogFrame) c->catalogFrameSize += received;
        else c->rxSize += received;
        bytes -= received;
        status=Drain(c,now,&frames);
        if (status!=UMI_STATUS_OK) return Fail(c,status,NULL);
    }
    if (c->nextIdReceived && !c->snapshot.clockReceived && !c->pingPending) {
        status=Ping(c,now);if(status!=UMI_STATUS_OK)return Fail(c,status,"Cannot queue API readiness probe.");
    }
    if (c->snapshot.state==UMI_IBKR_WAITING && c->accountsReceived && c->nextIdReceived && c->snapshot.clockReceived) {
        c->snapshot.state=UMI_IBKR_READY;c->snapshot.stale=false;
        (void)snprintf(c->snapshot.message,sizeof c->snapshot.message,"Read-only API ready. Confirm the account mode in TWS/Gateway; it is not attested here.");
    }
    if (c->snapshot.state==UMI_IBKR_READY && !c->pingPending && now-c->snapshot.clockReceivedAtMilliseconds>=30000U) {
        status=Ping(c,now);if(status!=UMI_STATUS_OK)return Fail(c,status,"Cannot queue heartbeat.");
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionReadAccount(UmiIbkrConnection *c,const char *account,uint64_t now)
{
    if (!c || !UmiIbkrText(account,64U,false) || now<c->lastNow) return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state!=UMI_IBKR_READY || c->snapshot.requestIssued) return UMI_STATUS_INVALID_STATE;
    bool found=false;for(size_t i=0;i<c->snapshot.accountCount;++i)if(strcmp(account,c->snapshot.accounts[i])==0)found=true;
    if (!found) return UMI_STATUS_PERMISSION_DENIED;
    /* Both requests have independent completion markers and are not an atomic
     * broker portfolio snapshot. Only the explicitly selected account is kept. */
    const char *summary[]={"62","1","35001","All","NetLiquidation,TotalCashValue,BuyingPower,AvailableFunds"};
    const char *positions[]={"61","1"};
    size_t before=c->txSize;
    UmiStatus status=UmiIbkrQueueFields(c,summary,5);
    if(status==UMI_STATUS_OK)status=UmiIbkrQueueFields(c,positions,2);
    if(status!=UMI_STATUS_OK){c->txSize=before;return status;}
    (void)snprintf(c->snapshot.selectedAccount,sizeof c->snapshot.selectedAccount,"%s",account);
    c->snapshot.requestIssued=true;c->snapshot.requestedAtMilliseconds=now;c->lastNow=now;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionCopy(const UmiIbkrConnection *c,UmiIbkrConnectionSnapshot *out)
{
    if(!c||!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=c->snapshot;return UMI_STATUS_OK;
}
void UmiIbkrConnectionClose(UmiIbkrConnection *c)
{
    if(!c)return;
    CloseIo(c);c->txSize=0;c->rxSize=0;c->snapshot.stale=true;
    if(c->snapshot.state!=UMI_IBKR_FAILED)c->snapshot.state=UMI_IBKR_DISCONNECTED;
}
void UmiIbkrConnectionDestroy(UmiIbkrConnection *c)
{
    if(!c)return;
    UmiIbkrConnectionClose(c);
    UmiIbkrOrdersDestroy(c);
    UmiIbkrCompletedDestroy(c);
    free(c->history);
    UmiIbkrDiscoveryDestroy(c);
    UmiIbkrScannerCatalogDestroy(c);
    for (size_t i=0; i<UMI_IBKR_REALTIME_STREAM_LIMIT; ++i) free(c->realtime[i]);
    if(c->DestroyIo)c->DestroyIo(c->io.context);
    free(c);
}
