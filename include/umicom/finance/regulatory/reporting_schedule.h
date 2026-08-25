/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_schedule.h
 *
 * PURPOSE:
 *   Define recurring due-date metadata for a regulatory report family.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_SCHEDULE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_SCHEDULE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingSchedule {
    char schedule_id[UMI_REG_ID_CAPACITY];
    char template_id[UMI_REG_ID_CAPACITY];
    uint32_t frequency_days;
    int32_t next_due_day;
} UmiReportingSchedule;

/* Initialise one validated define recurring due-date metadata for a regulatory report family. record. */
UmiStatus umi_reg_reporting_schedule_init(UmiReportingSchedule *record, const char *schedule_id, const char *template_id, uint32_t frequency_days, int32_t next_due_day);

#ifdef __cplusplus
}
#endif

#endif
