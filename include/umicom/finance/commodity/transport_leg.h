/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/transport_leg.h
 *
 * PURPOSE:
 *   Define one ordered physical leg within a commodity transport route.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_LEG_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_LEG_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityTransportLeg {
    UmiCommodityId route_id;
    uint32_t sequence;
    UmiCommodityId origin_location_id;
    UmiCommodityId destination_location_id;
    int64_t planned_departure_ms;
    int64_t planned_arrival_ms;
} UmiCommodityTransportLeg;

/* Initialise a bounded transport leg record for reusable Framework workflows. */
UmiStatus umi_commodity_transport_leg_init(UmiCommodityTransportLeg *value, const char *route_id, uint32_t sequence, const char *origin_location_id, const char *destination_location_id, int64_t planned_departure_ms, int64_t planned_arrival_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_transport_leg_valid(const UmiCommodityTransportLeg *value);

#ifdef __cplusplus
}
#endif

#endif
