/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/quantity_conversion.h
 *
 * PURPOSE:
 *   Convert commodity quantities using explicit rational unit factors.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_QUANTITY_CONVERSION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_QUANTITY_CONVERSION_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/unit_of_measure.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityQuantityConversion {
    char from_code[UMI_COMMODITY_CODE_CAPACITY];
    char to_code[UMI_COMMODITY_CODE_CAPACITY];
    int64_t numerator;
    int64_t denominator;
} UmiCommodityQuantityConversion;

/* Build a direct rational conversion between compatible units. */
UmiStatus umi_commodity_quantity_conversion_init(UmiCommodityQuantityConversion *value, const char *from_code, const char *to_code, int64_t numerator, int64_t denominator);

/* Convert integer minor units and reject results that would lose precision. */
UmiStatus umi_commodity_quantity_conversion_apply(const UmiCommodityQuantityConversion *value, int64_t input_units, int64_t *out_units);

#ifdef __cplusplus
}
#endif

#endif
