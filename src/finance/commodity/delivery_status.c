/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/delivery_status.c
 *
 * PURPOSE:
 *   Enforce deterministic legal transitions for physical delivery obligations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/delivery_status.h"

/* Encode lifecycle policy once in Framework rather than in each commodity product. */
bool umi_commodity_delivery_status_can_transition(UmiCommodityDeliveryState from, UmiCommodityDeliveryState to)
{
    if (from == to) return true;
    switch (from) {
        case UMI_COMMODITY_DELIVERY_PLANNED:
            return to == UMI_COMMODITY_DELIVERY_NOMINATED || to == UMI_COMMODITY_DELIVERY_CANCELLED;
        case UMI_COMMODITY_DELIVERY_NOMINATED:
            return to == UMI_COMMODITY_DELIVERY_IN_TRANSIT || to == UMI_COMMODITY_DELIVERY_FAILED || to == UMI_COMMODITY_DELIVERY_CANCELLED;
        case UMI_COMMODITY_DELIVERY_IN_TRANSIT:
            return to == UMI_COMMODITY_DELIVERY_DELIVERED || to == UMI_COMMODITY_DELIVERY_FAILED;
        default:
            return false;
    }
}

/* Terminal states protect downstream settlement from accidental reactivation. */
bool umi_commodity_delivery_status_terminal(UmiCommodityDeliveryState state)
{
    return state == UMI_COMMODITY_DELIVERY_DELIVERED || state == UMI_COMMODITY_DELIVERY_FAILED || state == UMI_COMMODITY_DELIVERY_CANCELLED;
}
