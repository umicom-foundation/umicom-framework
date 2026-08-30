/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/delivery_status.h
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_STATUS_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_STATUS_H

#include <stdbool.h>
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return true when a delivery lifecycle transition is allowed. */
bool umi_commodity_delivery_status_can_transition(UmiCommodityDeliveryState from, UmiCommodityDeliveryState to);

/* Return true when the state is terminal and must not progress further. */
bool umi_commodity_delivery_status_terminal(UmiCommodityDeliveryState state);

#ifdef __cplusplus
}
#endif

#endif
