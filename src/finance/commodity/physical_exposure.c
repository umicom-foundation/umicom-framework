/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/physical_exposure.c
 *
 * PURPOSE:
 *   Aggregate committed purchase and sale quantities into net physical exposure.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/physical_exposure.h"

#include <limits.h>
#include <string.h>

/* Initialise a neutral exposure record. */
UmiStatus umi_commodity_physical_exposure_init(UmiCommodityPhysicalExposure *value, const char *commodity_id, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    if (value == NULL || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->unit_code, sizeof value->unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Keep buy and sell commitments separately so gross exposure remains observable. */
UmiStatus umi_commodity_physical_exposure_add(UmiCommodityPhysicalExposure *value, int64_t purchase_units, int64_t sale_units)
{
    if (value == NULL || purchase_units < 0 || sale_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (purchase_units > INT64_MAX - value->purchase_units || sale_units > INT64_MAX - value->sale_units) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->purchase_units += purchase_units;
    value->sale_units += sale_units;
    return UMI_STATUS_OK;
}

/* Net exposure is derived from gross commitments. */
int64_t umi_commodity_physical_exposure_net(const UmiCommodityPhysicalExposure *value)
{
    if (value == NULL) return 0;
    return value->purchase_units - value->sale_units;
}
