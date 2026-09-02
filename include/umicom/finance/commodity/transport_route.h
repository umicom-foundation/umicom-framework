/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/transport_route.h
 *
 * PURPOSE:
 *   Define an auditable commodity logistics route between physical locations.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_ROUTE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_ROUTE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity transport route data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityTransportRoute {
    UmiCommodityId id;
    UmiCommodityId origin_location_id;
    UmiCommodityId destination_location_id;
    char mode_code[UMI_COMMODITY_CODE_CAPACITY];
    bool active;
} UmiCommodityTransportRoute;

/* Initialise a bounded transport route record for reusable Framework workflows. */
UmiStatus umi_commodity_transport_route_init(UmiCommodityTransportRoute *value, const char *id, const char *origin_location_id, const char *destination_location_id, const char *mode_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_transport_route_valid(const UmiCommodityTransportRoute *value);

#ifdef __cplusplus
}
#endif

#endif
