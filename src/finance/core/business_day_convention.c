/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/business_day_convention.c
 *
 * PURPOSE:
 *   Implement following and preceding date adjustment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/business_day_convention.h"

/* Seek to the next or previous valid business day with a bounded search. */
static UmiStatus seek(UmiFinancialDate d,const UmiHolidayCalendar *c,int32_t step,UmiFinancialDate *out){size_t n=0U;while(!umi_holiday_calendar_is_business_day(c,d)){if(++n>370U)return UMI_STATUS_INVALID_STATE;if(umi_business_date_add(d,step,&d)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;}*out=d;return UMI_STATUS_OK;}
/* Adjust date against calendar. */
UmiStatus umi_business_day_adjust(UmiFinancialDate d,const UmiHolidayCalendar *c,UmiBusinessDayConvention conv,UmiFinancialDate *out){UmiFinancialDate x;if(c==NULL||out==NULL||!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_ARGUMENT;if(conv==UMI_BUSINESS_DAY_UNADJUSTED||umi_holiday_calendar_is_business_day(c,d)){*out=d;return UMI_STATUS_OK;}if(conv==UMI_BUSINESS_DAY_FOLLOWING||conv==UMI_BUSINESS_DAY_MODIFIED_FOLLOWING){if(seek(d,c,1,&x)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(conv==UMI_BUSINESS_DAY_MODIFIED_FOLLOWING&&x.month!=d.month)return seek(d,c,-1,out);*out=x;return UMI_STATUS_OK;}if(conv==UMI_BUSINESS_DAY_PRECEDING||conv==UMI_BUSINESS_DAY_MODIFIED_PRECEDING){if(seek(d,c,-1,&x)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(conv==UMI_BUSINESS_DAY_MODIFIED_PRECEDING&&x.month!=d.month)return seek(d,c,1,out);*out=x;return UMI_STATUS_OK;}return UMI_STATUS_INVALID_ARGUMENT;}
