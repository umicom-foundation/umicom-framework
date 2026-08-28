/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/schedule_generator.h
 *
 * PURPOSE:
 *   Generate bounded deterministic schedules from financial rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SCHEDULE_GENERATOR_H
#define UMICOM_FINANCE_CORE_SCHEDULE_GENERATOR_H

#include "umicom/finance/core/schedule_period.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SCHEDULE_MAX 256U
typedef struct UmiSchedule { UmiSchedulePeriod periods[UMI_SCHEDULE_MAX]; size_t count; } UmiSchedule;
/* Generate forward schedule. */ UmiStatus umi_schedule_generate(const UmiScheduleRule *r,const UmiHolidayCalendar *c,UmiSchedule *out);
#ifdef __cplusplus
}
#endif

#endif
