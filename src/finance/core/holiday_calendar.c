/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/holiday_calendar.c
 *
 * PURPOSE:
 *   Implement holiday storage and weekend-aware business-day tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/holiday_calendar.h"

#include <string.h>
/* Initialize Saturday/Sunday weekend semantics. */
UmiStatus umi_holiday_calendar_init(UmiHolidayCalendar *c,const char *id){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);c->weekend_mask=(uint8_t)((1U<<5U)|(1U<<6U));return umi_financial_id_assign(&c->calendar_id,id);}
/* Add a unique holiday. */
UmiStatus umi_holiday_calendar_add(UmiHolidayCalendar *c,UmiFinancialDate d){size_t i;if(c==NULL||!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<c->count;i++)if(umi_financial_date_compare(c->holidays[i],d)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_HOLIDAY_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;c->holidays[c->count++]=d;return UMI_STATUS_OK;}
/* Test whether a date is a business day. */
bool umi_holiday_calendar_is_business_day(const UmiHolidayCalendar *c,UmiFinancialDate d){size_t i;uint8_t w;if(c==NULL||umi_business_date_weekday(d,&w)!=UMI_STATUS_OK)return false;if((c->weekend_mask&(uint8_t)(1U<<(w-1U)))!=0U)return false;for(i=0U;i<c->count;i++)if(umi_financial_date_compare(c->holidays[i],d)==0)return false;return true;}
