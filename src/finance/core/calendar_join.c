/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/calendar_join.c
 *
 * PURPOSE:
 *   Implement composed calendar business-day semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/calendar_join.h"

#include <string.h>
/* Initialize join. */ void umi_calendar_join_init(UmiCalendarJoin *j,UmiCalendarJoinMode mode){if(j!=NULL){memset(j,0,sizeof *j);j->mode=mode;}}
/* Add calendar. */ UmiStatus umi_calendar_join_add(UmiCalendarJoin *j,const UmiHolidayCalendar *c){if(j==NULL||c==NULL||j->mode>UMI_CALENDAR_ANY_OPEN)return UMI_STATUS_INVALID_ARGUMENT;if(j->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;j->items[j->count++]=c;return UMI_STATUS_OK;}
/* Test joined business day. */ bool umi_calendar_join_is_business_day(const UmiCalendarJoin *j,UmiFinancialDate d){size_t i;if(j==NULL||j->count==0U)return false;for(i=0U;i<j->count;i++){bool open=umi_holiday_calendar_is_business_day(j->items[i],d);if(j->mode==UMI_CALENDAR_ALL_OPEN&&!open)return false;if(j->mode==UMI_CALENDAR_ANY_OPEN&&open)return true;}return j->mode==UMI_CALENDAR_ALL_OPEN;}
