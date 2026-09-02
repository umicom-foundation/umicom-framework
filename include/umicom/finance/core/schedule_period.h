/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/schedule_period.h
 *
 * PURPOSE:
 *   Represent unadjusted and adjusted period boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SCHEDULE_PERIOD_H
#define UMICOM_FINANCE_CORE_SCHEDULE_PERIOD_H

#include "umicom/finance/core/schedule_rule.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the schedule period data shared with callers of this public contract.
 */
typedef struct UmiSchedulePeriod { UmiFinancialDate start; UmiFinancialDate end; UmiFinancialDate adjusted_start; UmiFinancialDate adjusted_end; size_t sequence; } UmiSchedulePeriod;
/* Initialize one adjusted period. */ UmiStatus umi_schedule_period_init(UmiSchedulePeriod *p,size_t seq,UmiFinancialDate a,UmiFinancialDate b,const UmiHolidayCalendar *c,UmiBusinessDayConvention conv);
#ifdef __cplusplus
}
#endif

#endif
