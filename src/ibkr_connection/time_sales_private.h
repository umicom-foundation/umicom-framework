/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/time_sales_private.h
 *
 * PURPOSE:
 *   Keep tick storage and protocol callbacks owned by the shared connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_TIME_SALES_PRIVATE_H
#define UMICOM_IBKR_TIME_SALES_PRIVATE_H
#include "umicom/broker_connectivity/time_sales.h"

typedef struct UmiIbkrTimeSalesStore {
    UmiIbkrTimeSalesSnapshot snapshot;
    size_t first;
    UmiIbkrTimeSalesEvent events[UMI_IBKR_TIME_SALES_EVENT_LIMIT];
} UmiIbkrTimeSalesStore;

UmiIbkrTimeSalesStore *UmiIbkrTimeSalesFind(const UmiIbkrConnection *, uint32_t);
void UmiIbkrTimeSalesClosed(UmiIbkrConnection *);
void UmiIbkrTimeSalesDestroy(UmiIbkrConnection *);
UmiStatus UmiIbkrTimeSalesFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
bool UmiIbkrTimeSalesProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
#endif
