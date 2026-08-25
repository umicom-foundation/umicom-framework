/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/grade_specification.h
 *
 * PURPOSE:
 *   Define a named quality grade tied to a canonical commodity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_GRADE_SPECIFICATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_GRADE_SPECIFICATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityGradeSpecification {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    char grade_code[UMI_COMMODITY_CODE_CAPACITY];
    bool active;
} UmiCommodityGradeSpecification;

/* Initialise a bounded grade specification record for reusable Framework workflows. */
UmiStatus umi_commodity_grade_specification_init(UmiCommodityGradeSpecification *value, const char *id, const char *commodity_id, const char *grade_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_grade_specification_valid(const UmiCommodityGradeSpecification *value);

#ifdef __cplusplus
}
#endif

#endif
