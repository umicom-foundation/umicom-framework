/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_private.h
 * PURPOSE: Keep captured bars off thread stacks and share private historical protocol helpers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_HISTORICAL_PRIVATE_H
#define UMICOM_IBKR_HISTORICAL_PRIVATE_H
#include "umicom/broker_connectivity/historical_bars.h"
typedef struct UmiIbkrHistoricalStore
{
    UmiIbkrHistoricalSnapshot snapshot;
    UmiIbkrHistoricalBar bars[UMI_IBKR_HISTORICAL_BAR_LIMIT];
} UmiIbkrHistoricalStore;
const char *UmiIbkrHistoricalBarSetting(uint32_t seconds);
const char *UmiIbkrHistoricalDataSetting(UmiIbkrHistoricalDataKind kind);
void UmiIbkrHistoricalExpire(UmiIbkrConnection *, uint64_t);
void UmiIbkrHistoricalConnectionClosed(UmiIbkrConnection *);
UmiStatus UmiIbkrHistoricalFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
bool UmiIbkrHistoricalProviderMessage(UmiIbkrConnection *, const char *, int, const char *);
#endif
