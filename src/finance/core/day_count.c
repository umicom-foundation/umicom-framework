/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/day_count.c
 *
 * PURPOSE:
 *   Implement ACT/360, ACT/365F and 30/360 fractions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/day_count.h"

/* Compute a supported year fraction. */
UmiStatus umi_day_count_fraction(UmiFinancialDate a,UmiFinancialDate b,UmiDayCountConvention c,double *out){int64_t x,y;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||umi_business_date_serial(a,&x)!=UMI_STATUS_OK||umi_business_date_serial(b,&y)!=UMI_STATUS_OK||y<x)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==UMI_DAY_COUNT_ACT_360)*out=(double)(y-x)/360.0;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==UMI_DAY_COUNT_ACT_365F)*out=(double)(y-x)/365.0;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==UMI_DAY_COUNT_THIRTY_360){int32_t d1=a.day>30U?30:(int32_t)a.day,d2=b.day>30U?30:(int32_t)b.day;*out=(double)((b.year-a.year)*360+((int32_t)b.month-(int32_t)a.month)*30+d2-d1)/360.0;}/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
