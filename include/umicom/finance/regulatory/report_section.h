/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_section.h
 *
 * PURPOSE:
 *   Define one ordered section in a regulatory report template.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_SECTION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_SECTION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportSection {
    char section_id[UMI_REG_ID_CAPACITY];
    char template_id[UMI_REG_ID_CAPACITY];
    char title[UMI_REG_NAME_CAPACITY];
    uint32_t ordinal;
} UmiReportSection;

/* Initialise one validated define one ordered section in a regulatory report template. record. */
UmiStatus umi_reg_report_section_init(UmiReportSection *record, const char *section_id, const char *template_id, const char *title, uint32_t ordinal);

#ifdef __cplusplus
}
#endif

#endif
