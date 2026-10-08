/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/realtime_private.h
 * PURPOSE: Keep streaming storage and protocol callbacks private to the shared connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_REALTIME_PRIVATE_H
#define UMICOM_IBKR_REALTIME_PRIVATE_H
#include "umicom/broker_connectivity/realtime_bars.h"
typedef struct UmiIbkrRealtimeStore
{
    UmiIbkrRealtimeSnapshot snapshot;
    size_t first;
    UmiIbkrRealtimeBar bars[UMI_IBKR_REALTIME_BAR_LIMIT];
} UmiIbkrRealtimeStore;
UmiIbkrRealtimeStore *UmiIbkrRealtimeFind(const UmiIbkrConnection *, uint32_t);
void UmiIbkrRealtimeConnectionClosed(UmiIbkrConnection *);
UmiStatus UmiIbkrRealtimeFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
bool UmiIbkrRealtimeProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
#endif
