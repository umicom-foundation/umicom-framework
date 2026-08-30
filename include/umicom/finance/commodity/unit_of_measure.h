/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/unit_of_measure.h
 *
 * PURPOSE:
 *   Define a physical unit with a conversion factor to its dimension base unit.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_UNIT_OF_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_UNIT_OF_MEASURE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityUnitOfMeasure {
    char code[UMI_COMMODITY_CODE_CAPACITY];
    char dimension[UMI_COMMODITY_CODE_CAPACITY];
    int64_t numerator;
    int64_t denominator;
    bool active;
} UmiCommodityUnitOfMeasure;

/* Initialise a bounded unit of measure record for reusable Framework workflows. */
UmiStatus umi_commodity_unit_of_measure_init(UmiCommodityUnitOfMeasure *value, const char *code, const char *dimension, int64_t numerator, int64_t denominator);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_unit_of_measure_valid(const UmiCommodityUnitOfMeasure *value);

#ifdef __cplusplus
}
#endif

#endif
