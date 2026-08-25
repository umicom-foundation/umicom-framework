/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_instance.h
 *
 * PURPOSE:
 *   Maintain bounded facts and lifecycle state for one regulatory report instance.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_INSTANCE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_INSTANCE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportInstance {
    char report_id[UMI_REG_ID_CAPACITY];
    char template_id[UMI_REG_ID_CAPACITY];
    UmiRegLifecycleState state;
    size_t fact_count;
    size_t mandatory_fact_count;
} UmiReportInstance;

/* Initialise a draft report instance for one template. */
UmiStatus umi_reg_report_instance_init(UmiReportInstance *report, const char *report_id, const char *template_id);
/* Record one validated fact and whether it satisfies a mandatory field. */
UmiStatus umi_reg_report_instance_record_fact(UmiReportInstance *report, int mandatory);
/* Return one when every recorded mandatory requirement is satisfied. */
int umi_reg_report_instance_complete(const UmiReportInstance *report, size_t required_mandatory_facts);

#ifdef __cplusplus
}
#endif

#endif
