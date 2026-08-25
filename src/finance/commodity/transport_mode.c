/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_mode.c
 *
 * PURPOSE:
 *   Implement a reusable physical transport mode such as vessel, pipeline, rail or truck.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/transport_mode.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_mode_init(UmiCommodityTransportMode *value, const char *code, const char *name, bool supports_bulk)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    if (status != UMI_STATUS_OK) return status;
    value->supports_bulk = supports_bulk;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_mode_valid(const UmiCommodityTransportMode *value)
{
    return value != NULL && (umi_commodity_text_valid(value->code) && umi_commodity_text_valid(value->name) && value->active);
}
