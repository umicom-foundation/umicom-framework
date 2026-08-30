/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/delivery_obligation.h
 *
 * PURPOSE:
 *   Define a contract delivery obligation with quantity, due time and lifecycle state.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_OBLIGATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_OBLIGATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityDeliveryObligation {
    UmiCommodityId id;
    UmiCommodityId contract_id;
    UmiCommodityQuantity quantity;
    int64_t due_time_ms;
    UmiCommodityDeliveryState state;
} UmiCommodityDeliveryObligation;

/* Initialise a bounded delivery obligation record for reusable Framework workflows. */
UmiStatus umi_commodity_delivery_obligation_init(UmiCommodityDeliveryObligation *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t due_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_delivery_obligation_valid(const UmiCommodityDeliveryObligation *value);

#ifdef __cplusplus
}
#endif

#endif
