/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/inventory_lot.h
 *
 * PURPOSE:
 *   Define an auditable physical inventory lot and its available quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_LOT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_LOT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityInventoryLot {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    UmiCommodityId facility_id;
    UmiCommodityQuantity quantity;
    int64_t received_time_ms;
    bool quality_accepted;
} UmiCommodityInventoryLot;

/* Initialise a bounded inventory lot record for reusable Framework workflows. */
UmiStatus umi_commodity_inventory_lot_init(UmiCommodityInventoryLot *value, const char *id, const char *commodity_id, const char *facility_id, int64_t units, int32_t scale, const char *unit_code, int64_t received_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_inventory_lot_valid(const UmiCommodityInventoryLot *value);

#ifdef __cplusplus
}
#endif

#endif
