/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_template.h
 *
 * PURPOSE:
 *   Define a versioned report template tied to one taxonomy.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_TEMPLATE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_TEMPLATE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportTemplate {
    char template_id[UMI_REG_ID_CAPACITY];
    char taxonomy_id[UMI_REG_ID_CAPACITY];
    char name[UMI_REG_NAME_CAPACITY];
    uint32_t version;
} UmiReportTemplate;

/* Initialise one validated define a versioned report template tied to one taxonomy. record. */
UmiStatus umi_reg_report_template_init(UmiReportTemplate *record, const char *template_id, const char *taxonomy_id, const char *name, uint32_t version);

#ifdef __cplusplus
}
#endif

#endif
