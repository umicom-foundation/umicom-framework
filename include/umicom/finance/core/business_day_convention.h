/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/business_day_convention.h
 *
 * PURPOSE:
 *   Adjust dates according to standard business-day conventions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BUSINESS_DAY_CONVENTION_H
#define UMICOM_FINANCE_CORE_BUSINESS_DAY_CONVENTION_H

#include "umicom/finance/core/holiday_calendar.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Adjust date against calendar. */ UmiStatus umi_business_day_adjust(UmiFinancialDate d,const UmiHolidayCalendar *c,UmiBusinessDayConvention conv,UmiFinancialDate *out);
#ifdef __cplusplus
}
#endif

#endif
