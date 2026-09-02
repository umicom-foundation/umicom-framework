/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/tenor.c
 *
 * PURPOSE:
 *   Implement compact tenor parsing and date application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/tenor.h"

#include <ctype.h>
#include <stdlib.h>
/* Parse compact tenor. */
UmiStatus umi_tenor_parse(const char *text,UmiTenor *out){char *e=NULL;unsigned long n;char u;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(text==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strtoul(text,&e,10);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==text||n==0UL||n>10000UL||e[0]=='\0'||e[1]!='\0')return UMI_STATUS_PARSE_ERROR;u=(char)toupper((unsigned char)e[0]);out->amount=(uint32_t)n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u=='D')out->unit=UMI_TENOR_DAYS;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u=='W')out->unit=UMI_TENOR_WEEKS;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u=='M')out->unit=UMI_TENOR_MONTHS;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(u=='Y')out->unit=UMI_TENOR_YEARS;/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_PARSE_ERROR;return UMI_STATUS_OK;}
/* Add tenor with month-end clamping. */
UmiStatus umi_tenor_add(UmiFinancialDate d,UmiTenor t,UmiFinancialDate *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||!umi_financial_date_is_valid(d)||t.amount==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t.unit==UMI_TENOR_DAYS)return umi_business_date_add(d,(int32_t)t.amount,out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t.unit==UMI_TENOR_WEEKS)return umi_business_date_add(d,(int32_t)(t.amount*7U),out);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t.unit==UMI_TENOR_YEARS)d.year+=(int32_t)t.amount;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(t.unit==UMI_TENOR_MONTHS){uint32_t total=(uint32_t)d.year*12U+(uint32_t)d.month-1U+t.amount;d.year=(int32_t)(total/12U);d.month=(uint8_t)(total%12U+1U);}/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(d.day>28U&&!umi_financial_date_is_valid(d))d.day--;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_STATE;*out=d;return UMI_STATUS_OK;}
