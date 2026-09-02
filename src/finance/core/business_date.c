/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/business_date.c
 *
 * PURPOSE:
 *   Implement integer Gregorian date conversion and arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/business_date.h"

/* Convert a Gregorian date to a serial day count using integer arithmetic. */
static int64_t days_from_civil(int32_t y,uint8_t m,uint8_t d){int64_t yy=y;int64_t mm=m;yy-=mm<=2?1:0;{int64_t era=(yy>=0?yy:yy-399)/400;uint32_t yoe=(uint32_t)(yy-era*400);uint32_t mp=(uint32_t)(mm+(mm>2?-3:9));uint32_t doy=(153U*mp+2U)/5U+(uint32_t)d-1U;uint32_t doe=yoe*365U+yoe/4U-yoe/100U+doy;return era*146097+(int64_t)doe-719468;}}
/* Convert a serial day count back to its Gregorian date components. */
static UmiFinancialDate civil_from_days(int64_t z){UmiFinancialDate d={0};z+=719468;{int64_t era=(z>=0?z:z-146096)/146097;uint32_t doe=(uint32_t)(z-era*146097);uint32_t yoe=(doe-doe/1460U+doe/36524U-doe/146096U)/365U;int64_t y=(int64_t)yoe+era*400;uint32_t doy=doe-(365U*yoe+yoe/4U-yoe/100U);uint32_t mp=(5U*doy+2U)/153U;uint32_t day=doy-(153U*mp+2U)/5U+1U;int32_t month=(int32_t)mp+(mp<10U?3:-9);y+=month<=2?1:0;d.year=(int32_t)y;d.month=(uint8_t)month;d.day=(uint8_t)day;}return d;}
/* Convert a valid date to serial days relative to 1970-01-01. */
UmiStatus umi_business_date_serial(UmiFinancialDate d,int64_t *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_ARGUMENT;*out=days_from_civil(d.year,d.month,d.day);return UMI_STATUS_OK;}
/* Convert serial days back to a valid date. */
UmiStatus umi_business_date_from_serial(int64_t serial,UmiFinancialDate *out){UmiFinancialDate d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;d=civil_from_days(serial);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_ARGUMENT;*out=d;return UMI_STATUS_OK;}
/* Add calendar days. */
UmiStatus umi_business_date_add(UmiFinancialDate d,int32_t days,UmiFinancialDate *out){int64_t s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_business_date_serial(d,&s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return umi_business_date_from_serial(s+(int64_t)days,out);}
/* Return Monday=1 through Sunday=7. */
UmiStatus umi_business_date_weekday(UmiFinancialDate d,uint8_t *out){int64_t s,r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||umi_business_date_serial(d,&s)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;r=(s+3)%7;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r<0)r+=7;*out=(uint8_t)(r+1);return UMI_STATUS_OK;}
