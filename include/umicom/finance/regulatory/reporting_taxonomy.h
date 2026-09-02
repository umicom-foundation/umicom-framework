/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_taxonomy.h
 *
 * PURPOSE:
 *   Describe a versioned reporting taxonomy controlled by an authority.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_TAXONOMY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_TAXONOMY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the reporting taxonomy data shared with callers of this public contract.
 */
typedef struct UmiReportingTaxonomy {
    char taxonomy_id[UMI_REG_ID_CAPACITY];
    char authority_id[UMI_REG_ID_CAPACITY];
    char version[UMI_REG_NAME_CAPACITY];
    int64_t effective_from_day;
} UmiReportingTaxonomy;

/* Initialise one validated describe a versioned reporting taxonomy controlled by an authority. record. */
UmiStatus umi_reg_reporting_taxonomy_init(UmiReportingTaxonomy *record, const char *taxonomy_id, const char *authority_id, const char *version, int64_t effective_from_day);

#ifdef __cplusplus
}
#endif

#endif
