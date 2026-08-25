/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/quality_specification.c
 *
 * PURPOSE:
 *   Implement a bounded set of quality measures for a commodity grade.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/quality_specification.h"

#include <string.h>

/* Initialise fixed storage so specification snapshots are allocation-free. */
UmiStatus umi_commodity_quality_specification_init(UmiCommodityQualitySpecification *value, const char *grade_id)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    return umi_commodity_copy_text(value->grade_id.value, sizeof value->grade_id.value, grade_id);
}

/* Add a measure only after validating its invariant bounds. */
UmiStatus umi_commodity_quality_specification_add(UmiCommodityQualitySpecification *value, const UmiCommodityQualityMeasure *measure)
{
    if (value == NULL || measure == NULL || !umi_commodity_quality_measure_valid(measure)) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->count >= UMI_COMMODITY_MAX_QUALITY_MEASURES) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->measures[value->count++] = *measure;
    return UMI_STATUS_OK;
}

/* Require useful content before quality data is used for delivery decisions. */
bool umi_commodity_quality_specification_valid(const UmiCommodityQualitySpecification *value)
{
    return value != NULL && umi_commodity_text_valid(value->grade_id.value) && value->count > 0U;
}
