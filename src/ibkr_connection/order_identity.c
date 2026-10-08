/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_identity.c
 * PURPOSE: Retain broker order identities without reserving or submitting an order.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>

UmiStatus UmiIbkrOrdersNextId(UmiIbkrConnection *c, uint32_t next)
{
    if (next > INT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrOrderIdentitySnapshot *id = &c->orders.identity;
    /* Repeated readiness callbacks can move backwards. Keep the latest broker
     * report for display, while the conservative floor never decreases. */
    id->brokerIdReceived = true;
    id->brokerNextOrderId = next;
    if (next > id->conservativeNextOrderId)
        id->conservativeNextOrderId = next;
    c->nextIdReceived = true;
    return UMI_STATUS_OK;
}
void UmiIbkrOrdersObserveId(UmiIbkrConnection *c, int32_t observed)
{
    if (observed < 0)
        return;
    UmiIbkrOrderIdentitySnapshot *id = &c->orders.identity;
    uint32_t value = (uint32_t)observed;
    if (!id->observedIdReceived || value > id->highestObservedOrderId)
        id->highestObservedOrderId = value;
    id->observedIdReceived = true;
    if (value == INT32_MAX)
    {
        id->exhausted = true;
        return;
    }
    if (value + 1U > id->conservativeNextOrderId)
        id->conservativeNextOrderId = value + 1U;
}
