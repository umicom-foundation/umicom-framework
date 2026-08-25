/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/commodity_snapshot.c
 *
 * PURPOSE:
 *   Capture bounded aggregate physical-contract, inventory, shipment and nomination evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/commodity_snapshot.h"

#include <string.h>

/* Initialise snapshot evidence without taking ownership of domain books. */
void umi_commodity_commodity_snapshot_init(UmiCommoditySnapshot *value, int64_t captured_time_ms)
{
    if (value != NULL) {
        memset(value, 0, sizeof *value);
        value->captured_time_ms = captured_time_ms;
        value->revision = 1U;
    }
}

/* Snapshot validity is intentionally independent of whether inventories are empty. */
bool umi_commodity_commodity_snapshot_valid(const UmiCommoditySnapshot *value)
{
    return value != NULL && value->captured_time_ms >= 0 && value->revision > 0U;
}
