/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/types.c
 *
 * PURPOSE:
 *   Implement shared bounded text, identifier and Gregorian-date helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/types.h"

#include <string.h>
/* Determine whether a Gregorian year is a leap year. */
static bool leap(int32_t y){return (y%4==0)&&((y%100!=0)||(y%400==0));}
/* Copy text into a bounded caller-owned buffer. */
UmiStatus umi_financial_core_copy(char *dst,size_t cap,const char *src){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dst==NULL||src==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(src);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>=cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(dst,src,n+1U);return UMI_STATUS_OK;}
/* Assign a reusable existing financial identifier. */
UmiStatus umi_financial_id_assign(UmiFinancialId *id,const char *value){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(id==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_financial_core_copy(id->value,sizeof id->value,value);}
/* Test whether a reusable financial identifier is populated. */
bool umi_financial_id_is_valid(const UmiFinancialId *id){return id!=NULL&&id->value[0]!='\0';}
/* Compare reusable financial identifiers lexically. */
int umi_financial_id_compare(const UmiFinancialId *a,const UmiFinancialId *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL&&b==NULL)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL)return -1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL)return 1;return strcmp(a->value,b->value);}
/* Validate a Gregorian financial date. */
bool umi_financial_date_is_valid(UmiFinancialDate d){static const uint8_t md[12]={31U,28U,31U,30U,31U,30U,31U,31U,30U,31U,30U,31U};uint8_t m;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.year<1600||d.year>9999||d.month<1U||d.month>12U)return false;m=md[(size_t)d.month-1U];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.month==2U&&leap(d.year))m=29U;return d.day>=1U&&d.day<=m;}
/* Compare two financial dates. */
int umi_financial_date_compare(UmiFinancialDate a,UmiFinancialDate b){/* Apply this branch only when its contract condition is satisfied. */ if(a.year!=b.year)return a.year<b.year?-1:1;/* Apply this branch only when its contract condition is satisfied. */ if(a.month!=b.month)return a.month<b.month?-1:1;/* Apply this branch only when its contract condition is satisfied. */ if(a.day!=b.day)return a.day<b.day?-1:1;return 0;}
