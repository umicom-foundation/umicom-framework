/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/shipment.h
 *
 * PURPOSE:
 *   Define a physical shipment and its planned versus delivered quantity.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_SHIPMENT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_SHIPMENT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity shipment data shared with callers of this public contract.
 */
typedef struct UmiCommodityShipment {
    UmiCommodityId id;
    UmiCommodityId contract_id;
    UmiCommodityId route_id;
    UmiCommodityQuantity planned_quantity;
    int64_t delivered_units;
    UmiCommodityShipmentState state;
} UmiCommodityShipment;

/* Initialise a bounded shipment record for reusable Framework workflows. */
UmiStatus umi_commodity_shipment_init(UmiCommodityShipment *value, const char *id, const char *contract_id, const char *route_id, int64_t planned_units, int32_t scale, const char *unit_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_shipment_valid(const UmiCommodityShipment *value);

#ifdef __cplusplus
}
#endif

#endif
