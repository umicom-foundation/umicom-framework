/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_cost.c
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

#include "umicom/finance/commodity/transport_cost.h"

#include <limits.h>
#include <string.h>

/* Store transport economics independently from a broker, carrier or application. */
UmiStatus umi_commodity_transport_cost_init(UmiCommodityTransportCost *value, const char *route_id, int64_t base_minor_units, int64_t per_unit_minor_units, const UmiCurrency *currency)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || currency == NULL || base_minor_units < 0 || per_unit_minor_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->route_id.value, sizeof value->route_id.value, route_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->base_minor_units = base_minor_units;
    value->per_unit_minor_units = per_unit_minor_units;
    value->currency = *currency;
    return UMI_STATUS_OK;
}

/* Protect deterministic integer cost calculation from overflow. */
UmiStatus umi_commodity_transport_cost_calculate(const UmiCommodityTransportCost *value, int64_t units, int64_t *out_minor_units)
{
    int64_t variable;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_minor_units == NULL || units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (units != 0 && value->per_unit_minor_units > INT64_MAX / units) return UMI_STATUS_CAPACITY_EXCEEDED;
    variable = units * value->per_unit_minor_units;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (variable > INT64_MAX - value->base_minor_units) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_minor_units = value->base_minor_units + variable;
    return UMI_STATUS_OK;
}
