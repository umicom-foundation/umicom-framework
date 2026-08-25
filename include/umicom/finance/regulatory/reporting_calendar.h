/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_calendar.h
 *
 * PURPOSE:
 *   Define due-date policy for a regulatory reporting cycle.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_CALENDAR_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_CALENDAR_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingCalendar {
    char calendar_id[UMI_REG_ID_CAPACITY];
    char authority_id[UMI_REG_ID_CAPACITY];
    uint32_t frequency_days;
    uint32_t grace_days;
} UmiReportingCalendar;

/* Initialise one validated define due-date policy for a regulatory reporting cycle. record. */
UmiStatus umi_reg_reporting_calendar_init(UmiReportingCalendar *record, const char *calendar_id, const char *authority_id, uint32_t frequency_days, uint32_t grace_days);

#ifdef __cplusplus
}
#endif

#endif
