/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/schedule_generator.c
 *
 * PURPOSE:
 *   Implement bounded forward financial schedule generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/schedule_generator.h"

#include <string.h>
/* Generate forward schedule. */
UmiStatus umi_schedule_generate(const UmiScheduleRule *r,const UmiHolidayCalendar *c,UmiSchedule *out){UmiFinancialDate a,b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_schedule_rule_is_valid(r)||c==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof *out);a=r->start;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(umi_financial_date_compare(a,r->end)<0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out->count>=UMI_SCHEDULE_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_tenor_add(a,r->frequency,&b)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_date_compare(b,r->end)>0)b=r->end;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_schedule_period_init(&out->periods[out->count],out->count,a,b,c,r->convention)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;out->count++;a=b;}return out->count>0U?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
