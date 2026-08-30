/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/taxonomy_dimension.h
 *
 * PURPOSE:
 *   Describe one reporting dimension used to qualify taxonomy facts.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_DIMENSION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_DIMENSION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTaxonomyDimension {
    char dimension_id[UMI_REG_ID_CAPACITY];
    char taxonomy_id[UMI_REG_ID_CAPACITY];
    char label[UMI_REG_NAME_CAPACITY];
    int required;
} UmiTaxonomyDimension;

/* Initialise one validated describe one reporting dimension used to qualify taxonomy facts. record. */
UmiStatus umi_reg_taxonomy_dimension_init(UmiTaxonomyDimension *record, const char *dimension_id, const char *taxonomy_id, const char *label, int required);

#ifdef __cplusplus
}
#endif

#endif
