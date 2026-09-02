/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/transport_cost.h
 *
 * PURPOSE:
 *   Calculate deterministic route transport charges from base and per-unit tariffs.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_COST_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_COST_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity transport cost data shared with callers of this public contract.
 */
typedef struct UmiCommodityTransportCost {
    UmiCommodityId route_id;
    int64_t base_minor_units;
    int64_t per_unit_minor_units;
    UmiCurrency currency;
} UmiCommodityTransportCost;

/* Initialise a transport tariff with non-negative cost components. */
UmiStatus umi_commodity_transport_cost_init(UmiCommodityTransportCost *value, const char *route_id, int64_t base_minor_units, int64_t per_unit_minor_units, const UmiCurrency *currency);

/* Calculate transport cost for whole normalised quantity units. */
UmiStatus umi_commodity_transport_cost_calculate(const UmiCommodityTransportCost *value, int64_t units, int64_t *out_minor_units);

#ifdef __cplusplus
}
#endif

#endif
