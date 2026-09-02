/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/commodity_service.c
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

#include "umicom/finance/commodity/commodity_service.h"

#include <string.h>

/* Initialise every component so thin applications never own hidden service state. */
void umi_commodity_commodity_service_init(UmiCommodityService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service != NULL) {
        memset(service, 0, sizeof *service);
        umi_commodity_commodity_catalogue_init(&service->commodities);
        umi_commodity_contract_book_init(&service->contracts);
        umi_commodity_inventory_book_init(&service->inventories);
        umi_commodity_shipment_book_init(&service->shipments);
        umi_commodity_nomination_book_init(&service->nominations);
        service->revision = 1U;
    }
}

/* Start is explicit so dependency injection and runtime health can govern activation. */
UmiStatus umi_commodity_commodity_service_start(UmiCommodityService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->started = true;
    service->revision++;
    return UMI_STATUS_OK;
}

/* Snapshot counts provide toolkit-neutral evidence for Desk, TMS and operations views. */
UmiStatus umi_commodity_commodity_service_snapshot(const UmiCommodityService *service, int64_t captured_time_ms, UmiCommoditySnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL || !service->started || captured_time_ms < 0) return UMI_STATUS_INVALID_STATE;
    umi_commodity_commodity_snapshot_init(out_snapshot, captured_time_ms);
    out_snapshot->commodity_count = service->commodities.count;
    out_snapshot->contract_count = service->contracts.count;
    out_snapshot->inventory_count = service->inventories.count;
    out_snapshot->shipment_count = service->shipments.count;
    out_snapshot->nomination_count = service->nominations.count;
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}
