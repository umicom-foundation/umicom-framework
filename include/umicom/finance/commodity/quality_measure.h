/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/quality_measure.h
 *
 * PURPOSE:
 *   Define an inclusive numeric quality requirement such as density or sulphur.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_MEASURE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityQualityMeasure {
    char name[UMI_COMMODITY_NAME_CAPACITY];
    char unit_code[UMI_COMMODITY_CODE_CAPACITY];
    int64_t minimum;
    int64_t maximum;
    int32_t scale;
} UmiCommodityQualityMeasure;

/* Initialise a bounded quality measure record for reusable Framework workflows. */
UmiStatus umi_commodity_quality_measure_init(UmiCommodityQualityMeasure *value, const char *name, const char *unit_code, int64_t minimum, int64_t maximum, int32_t scale);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_quality_measure_valid(const UmiCommodityQualityMeasure *value);

#ifdef __cplusplus
}
#endif

#endif
