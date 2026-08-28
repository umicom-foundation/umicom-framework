/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/schedule_period.c
 *
 * PURPOSE:
 *   Implement adjusted schedule period construction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/schedule_period.h"

/* Initialize one adjusted period. */ UmiStatus umi_schedule_period_init(UmiSchedulePeriod *p,size_t seq,UmiFinancialDate a,UmiFinancialDate b,const UmiHolidayCalendar *c,UmiBusinessDayConvention conv){if(p==NULL||c==NULL||umi_financial_date_compare(a,b)>=0)return UMI_STATUS_INVALID_ARGUMENT;p->start=a;p->end=b;p->sequence=seq;if(umi_business_day_adjust(a,c,conv,&p->adjusted_start)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;return umi_business_day_adjust(b,c,conv,&p->adjusted_end);}
