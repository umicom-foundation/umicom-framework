/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_job.h
 *
 * PURPOSE:
 *   Represent one scheduled regulatory reporting work item.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_JOB_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_JOB_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingJob {
    char job_id[UMI_REG_ID_CAPACITY];
    char report_id[UMI_REG_ID_CAPACITY];
    uint32_t priority;
    int64_t due_ms;
} UmiReportingJob;

/* Initialise one validated represent one scheduled regulatory reporting work item. record. */
UmiStatus umi_reg_reporting_job_init(UmiReportingJob *record, const char *job_id, const char *report_id, uint32_t priority, int64_t due_ms);

#ifdef __cplusplus
}
#endif

#endif
