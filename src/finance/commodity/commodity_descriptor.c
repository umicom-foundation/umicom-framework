/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/commodity_descriptor.c
 *
 * PURPOSE:
 *   Implement reference-data metadata for a physical or financially settled commodity.
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

#include "umicom/finance/commodity/commodity_descriptor.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_commodity_descriptor_init(UmiCommodityDescriptor *value, const char *id, const char *name, const char *code, UmiCommodityKind kind, const UmiCurrency *currency, bool physical_delivery)
{
    UmiStatus status;
    if (value == NULL || currency == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    if (status != UMI_STATUS_OK) return status;
    value->kind = kind;
    value->settlement_currency = *currency;
    value->physical_delivery = physical_delivery;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_commodity_descriptor_valid(const UmiCommodityDescriptor *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->code) && value->kind != UMI_COMMODITY_KIND_UNKNOWN && value->active);
}
