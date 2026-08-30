/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/shipment_book.h
 *
 * PURPOSE:
 *   Define a bounded book of physical commodity shipments.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_SHIPMENT_BOOK_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_SHIPMENT_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/shipment.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityShipmentBook {
    UmiCommodityShipment items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityShipmentBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_shipment_book_init(UmiCommodityShipmentBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_shipment_book_add(UmiCommodityShipmentBook *catalogue, const UmiCommodityShipment *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityShipment *umi_commodity_shipment_book_find(const UmiCommodityShipmentBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
