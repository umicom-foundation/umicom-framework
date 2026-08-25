/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/quantity_conversion.c
 *
 * PURPOSE:
 *   Convert commodity quantities using explicit rational unit factors.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/quantity_conversion.h"

#include <limits.h>
#include <string.h>

/* Store a rational conversion so callers can audit the exact factor used. */
UmiStatus umi_commodity_quantity_conversion_init(UmiCommodityQuantityConversion *value, const char *from_code, const char *to_code, int64_t numerator, int64_t denominator)
{
    UmiStatus status;
    if (value == NULL || numerator <= 0 || denominator <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->from_code, sizeof value->from_code, from_code);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->to_code, sizeof value->to_code, to_code);
    if (status != UMI_STATUS_OK) return status;
    value->numerator = numerator;
    value->denominator = denominator;
    return UMI_STATUS_OK;
}

/* Require exact divisibility so financial quantity conversion never silently rounds. */
UmiStatus umi_commodity_quantity_conversion_apply(const UmiCommodityQuantityConversion *value, int64_t input_units, int64_t *out_units)
{
    int64_t product;
    if (value == NULL || out_units == NULL || value->numerator <= 0 || value->denominator <= 0 || input_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (input_units != 0 && value->numerator > INT64_MAX / input_units) return UMI_STATUS_CAPACITY_EXCEEDED;
    product = input_units * value->numerator;
    if ((product % value->denominator) != 0) return UMI_STATUS_INVALID_STATE;
    *out_units = product / value->denominator;
    return UMI_STATUS_OK;
}
