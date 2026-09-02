/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/unit_of_measure.c
 *
 * PURPOSE:
 *   Implement a physical unit with a conversion factor to its dimension base unit.
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

#include "umicom/finance/commodity/unit_of_measure.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_unit_of_measure_init(UmiCommodityUnitOfMeasure *value, const char *code, const char *dimension, int64_t numerator, int64_t denominator)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || numerator <= 0 || denominator <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->dimension, sizeof value->dimension, dimension);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->numerator = numerator;
    value->denominator = denominator;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_unit_of_measure_valid(const UmiCommodityUnitOfMeasure *value)
{
    return value != NULL && (umi_commodity_text_valid(value->code) && umi_commodity_text_valid(value->dimension) && value->numerator > 0 && value->denominator > 0 && value->active);
}
