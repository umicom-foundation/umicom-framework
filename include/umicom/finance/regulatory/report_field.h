/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_field.h
 *
 * PURPOSE:
 *   Map a report field to a taxonomy concept and section.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_FIELD_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_FIELD_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the report field data shared with callers of this public contract.
 */
typedef struct UmiReportField {
    char field_id[UMI_REG_ID_CAPACITY];
    char section_id[UMI_REG_ID_CAPACITY];
    char concept_id[UMI_REG_ID_CAPACITY];
    int mandatory;
} UmiReportField;

/* Initialise one validated map a report field to a taxonomy concept and section. record. */
UmiStatus umi_reg_report_field_init(UmiReportField *record, const char *field_id, const char *section_id, const char *concept_id, int mandatory);

#ifdef __cplusplus
}
#endif

#endif
