/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/location.c
 *
 * PURPOSE:
 *   Implement a physical delivery, storage or logistics location.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/location.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_location_init(UmiCommodityLocation *value, const char *id, const char *name, const char *country_code)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->country_code, sizeof value->country_code, country_code);
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_location_valid(const UmiCommodityLocation *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->name) && strlen(value->country_code) == 2U && value->active);
}
