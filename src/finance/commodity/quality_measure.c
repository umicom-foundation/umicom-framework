/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/quality_measure.c
 *
 * PURPOSE:
 *   Implement an inclusive numeric quality requirement such as density or sulphur.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/quality_measure.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_quality_measure_init(UmiCommodityQualityMeasure *value, const char *name, const char *unit_code, int64_t minimum, int64_t maximum, int32_t scale)
{
    UmiStatus status;
    if (value == NULL || minimum > maximum || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->unit_code, sizeof value->unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->minimum = minimum;
    value->maximum = maximum;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_quality_measure_valid(const UmiCommodityQualityMeasure *value)
{
    return value != NULL && (umi_commodity_text_valid(value->name) && umi_commodity_text_valid(value->unit_code) && value->minimum <= value->maximum && value->scale >= 0);
}
