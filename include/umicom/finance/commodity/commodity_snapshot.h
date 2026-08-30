/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/commodity_snapshot.h
 *
 * PURPOSE:
 *   Capture bounded aggregate physical-contract, inventory, shipment and nomination evidence.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommoditySnapshot {
    size_t commodity_count;
    size_t contract_count;
    size_t inventory_count;
    size_t shipment_count;
    size_t nomination_count;
    int64_t captured_time_ms;
    uint64_t revision;
} UmiCommoditySnapshot;

/* Initialise a point-in-time commodity platform snapshot. */
void umi_commodity_commodity_snapshot_init(UmiCommoditySnapshot *value, int64_t captured_time_ms);

/* Return true when the snapshot has a valid timestamp and revision. */
bool umi_commodity_commodity_snapshot_valid(const UmiCommoditySnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
