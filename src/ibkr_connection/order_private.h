/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_private.h
 * PURPOSE: Keep retained order payloads and observation mutations inside their connection owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_ORDER_PRIVATE_H
#define UMICOM_IBKR_ORDER_PRIVATE_H
#include "umicom/broker_connectivity/order_recovery.h"
typedef struct UmiIbkrOrderStore
{
    UmiIbkrOrderRecoverySnapshot snapshot;
    UmiIbkrOrderIdentitySnapshot identity;
    UmiIbkrRecoveredOrder rows[UMI_IBKR_ORDER_LIMIT];
    unsigned char *payloads[UMI_IBKR_ORDER_LIMIT];
    size_t lengths[UMI_IBKR_ORDER_LIMIT];
} UmiIbkrOrderStore;
UmiStatus UmiIbkrOrdersFrame(UmiIbkrConnection *, uint64_t, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrOrdersNextId(UmiIbkrConnection *, uint32_t);
void UmiIbkrOrdersObserveId(UmiIbkrConnection *, int32_t);
void UmiIbkrOrdersExpire(UmiIbkrConnection *, uint64_t);
void UmiIbkrOrdersDestroy(UmiIbkrConnection *);
UmiStatus UmiIbkrOrdersStore(UmiIbkrConnection *, const UmiIbkrRecoveredOrder *, const unsigned char *,
                             size_t, uint64_t);
#endif
