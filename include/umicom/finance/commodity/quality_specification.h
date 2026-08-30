/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/quality_specification.h
 *
 * PURPOSE:
 *   Define a bounded set of quality measures for a commodity grade.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_SPECIFICATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_SPECIFICATION_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/quality_measure.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_COMMODITY_MAX_QUALITY_MEASURES 16U

typedef struct UmiCommodityQualitySpecification {
    UmiCommodityId grade_id;
    UmiCommodityQualityMeasure measures[UMI_COMMODITY_MAX_QUALITY_MEASURES];
    size_t count;
} UmiCommodityQualitySpecification;

/* Initialise the quality specification for one grade. */
UmiStatus umi_commodity_quality_specification_init(UmiCommodityQualitySpecification *value, const char *grade_id);

/* Append a validated quality measure while preserving declared order. */
UmiStatus umi_commodity_quality_specification_add(UmiCommodityQualitySpecification *value, const UmiCommodityQualityMeasure *measure);

/* Validate that a specification has a grade and at least one valid measure. */
bool umi_commodity_quality_specification_valid(const UmiCommodityQualitySpecification *value);

#ifdef __cplusplus
}
#endif

#endif
