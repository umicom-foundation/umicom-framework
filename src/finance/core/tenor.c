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
UmiStatus umi_tenor_parse(const char *text,UmiTenor *out){char *e=NULL;unsigned long n;char u;if(text==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strtoul(text,&e,10);if(e==text||n==0UL||n>10000UL||e[0]=='\0'||e[1]!='\0')return UMI_STATUS_PARSE_ERROR;u=(char)toupper((unsigned char)e[0]);out->amount=(uint32_t)n;if(u=='D')out->unit=UMI_TENOR_DAYS;else if(u=='W')out->unit=UMI_TENOR_WEEKS;else if(u=='M')out->unit=UMI_TENOR_MONTHS;else if(u=='Y')out->unit=UMI_TENOR_YEARS;else return UMI_STATUS_PARSE_ERROR;return UMI_STATUS_OK;}
/* Add tenor with month-end clamping. */
UmiStatus umi_tenor_add(UmiFinancialDate d,UmiTenor t,UmiFinancialDate *out){if(out==NULL||!umi_financial_date_is_valid(d)||t.amount==0U)return UMI_STATUS_INVALID_ARGUMENT;if(t.unit==UMI_TENOR_DAYS)return umi_business_date_add(d,(int32_t)t.amount,out);if(t.unit==UMI_TENOR_WEEKS)return umi_business_date_add(d,(int32_t)(t.amount*7U),out);if(t.unit==UMI_TENOR_YEARS)d.year+=(int32_t)t.amount;else if(t.unit==UMI_TENOR_MONTHS){uint32_t total=(uint32_t)d.year*12U+(uint32_t)d.month-1U+t.amount;d.year=(int32_t)(total/12U);d.month=(uint8_t)(total%12U+1U);}else return UMI_STATUS_INVALID_ARGUMENT;while(d.day>28U&&!umi_financial_date_is_valid(d))d.day--;if(!umi_financial_date_is_valid(d))return UMI_STATUS_INVALID_STATE;*out=d;return UMI_STATUS_OK;}
