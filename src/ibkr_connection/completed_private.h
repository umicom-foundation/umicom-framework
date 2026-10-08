/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_private.h
 * PURPOSE: Keep completed-order storage and decoding private to the broker connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_COMPLETED_PRIVATE_H
#define UMICOM_IBKR_COMPLETED_PRIVATE_H
#include "umicom/broker_connectivity/completed_orders.h"
typedef struct UmiIbkrCompletedStore
{
    UmiIbkrCompletedOrdersSnapshot snapshot;
    UmiIbkrCompletedOrder rows[UMI_IBKR_COMPLETED_ORDER_LIMIT];
    unsigned char *payloads[UMI_IBKR_COMPLETED_ORDER_LIMIT];
    size_t lengths[UMI_IBKR_COMPLETED_ORDER_LIMIT];
} UmiIbkrCompletedStore;
UmiStatus UmiIbkrCompletedFrame(UmiIbkrConnection *, uint64_t, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrCompletedDecode(char **, size_t, int, UmiIbkrCompletedOrder *);
UmiStatus UmiIbkrCompletedStoreRow(UmiIbkrConnection *, const UmiIbkrCompletedOrder *, const unsigned char *,
                                   size_t, uint64_t);
void UmiIbkrCompletedExpire(UmiIbkrConnection *, uint64_t);
void UmiIbkrCompletedDestroy(UmiIbkrConnection *);
#endif
