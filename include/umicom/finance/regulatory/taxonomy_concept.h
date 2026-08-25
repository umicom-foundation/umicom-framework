/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/taxonomy_concept.h
 *
 * PURPOSE:
 *   Describe one typed concept within a regulatory taxonomy.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_CONCEPT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_TAXONOMY_CONCEPT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTaxonomyConcept {
    char concept_id[UMI_REG_ID_CAPACITY];
    char taxonomy_id[UMI_REG_ID_CAPACITY];
    char label[UMI_REG_NAME_CAPACITY];
    int numeric;
} UmiTaxonomyConcept;

/* Initialise one validated describe one typed concept within a regulatory taxonomy. record. */
UmiStatus umi_reg_taxonomy_concept_init(UmiTaxonomyConcept *record, const char *concept_id, const char *taxonomy_id, const char *label, int numeric);

#ifdef __cplusplus
}
#endif

#endif
