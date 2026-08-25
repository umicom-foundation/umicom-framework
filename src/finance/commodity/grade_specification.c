/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/grade_specification.c
 *
 * PURPOSE:
 *   Implement a named quality grade tied to a canonical commodity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/grade_specification.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_grade_specification_init(UmiCommodityGradeSpecification *value, const char *id, const char *commodity_id, const char *grade_code)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->grade_code, sizeof value->grade_code, grade_code);
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_grade_specification_valid(const UmiCommodityGradeSpecification *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->commodity_id.value) && umi_commodity_text_valid(value->grade_code) && value->active);
}
