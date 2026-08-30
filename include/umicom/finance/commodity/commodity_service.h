/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/commodity_service.h
 *
 * PURPOSE:
 *   Aggregate commodity reference data and operational readiness into one Framework service boundary.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/finance/commodity/commodity_catalogue.h"
#include "umicom/finance/commodity/contract_book.h"
#include "umicom/finance/commodity/inventory_book.h"
#include "umicom/finance/commodity/shipment_book.h"
#include "umicom/finance/commodity/nomination_book.h"
#include "umicom/finance/commodity/commodity_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityService {
    UmiCommodityCatalogue commodities;
    UmiCommodityContractBook contracts;
    UmiCommodityInventoryBook inventories;
    UmiCommodityShipmentBook shipments;
    UmiCommodityNominationBook nominations;
    bool started;
    uint64_t revision;
} UmiCommodityService;

/* Initialise the Framework commodity service and all bounded registries. */
void umi_commodity_commodity_service_init(UmiCommodityService *service);

/* Mark the service started only when its reusable reference-data boundary is usable. */
UmiStatus umi_commodity_commodity_service_start(UmiCommodityService *service);

/* Build aggregate operational evidence without exposing mutable registries. */
UmiStatus umi_commodity_commodity_service_snapshot(const UmiCommodityService *service, int64_t captured_time_ms, UmiCommoditySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
