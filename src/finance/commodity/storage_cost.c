/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/storage_cost.c
 *
 * PURPOSE:
 *   Calculate deterministic storage charges from daily rates and occupied units.
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

#include "umicom/finance/commodity/storage_cost.h"

#include <limits.h>
#include <string.h>

/* Keep tariff configuration explicit and reusable across physical applications. */
UmiStatus umi_commodity_storage_cost_init(UmiCommodityStorageCost *value, const char *facility_id, int64_t minor_units_per_day_per_unit, const UmiCurrency *currency)
{
    UmiStatus status;
    if (value == NULL || currency == NULL || minor_units_per_day_per_unit < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->facility_id.value, sizeof value->facility_id.value, facility_id);
    if (status != UMI_STATUS_OK) return status;
    value->minor_units_per_day_per_unit = minor_units_per_day_per_unit;
    value->currency = *currency;
    return UMI_STATUS_OK;
}

/* Use integer arithmetic so cost calculations remain deterministic. */
UmiStatus umi_commodity_storage_cost_calculate(const UmiCommodityStorageCost *value, int64_t units, int32_t days, int64_t *out_minor_units)
{
    int64_t daily;
    if (value == NULL || out_minor_units == NULL || units < 0 || days < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (units != 0 && value->minor_units_per_day_per_unit > INT64_MAX / units) return UMI_STATUS_CAPACITY_EXCEEDED;
    daily = units * value->minor_units_per_day_per_unit;
    if (days != 0 && daily > INT64_MAX / days) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_minor_units = daily * days;
    return UMI_STATUS_OK;
}
