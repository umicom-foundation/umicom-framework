/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_result.c
 *
 * PURPOSE:
 *   Own bounded key/value rows returned by analytical pipelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_result.h"

#include <string.h>
/*
 * Initialise analytics result from caller-provided values so later operations receive a
 * known state.
 */
void umi_analytics_result_init(UmiAnalyticsResult *result){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result!=NULL)memset(result,0,sizeof *result);}
/* Add analytics result only after its inputs and available capacity have been checked. */
UmiStatus umi_analytics_result_add(UmiAnalyticsResult *result,const char *key,double value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result->count>=UMI_ANALYTICS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_analytics_copy_text(result->rows[result->count].key,sizeof result->rows[result->count].key,key);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;result->rows[result->count].value=value;result->count++;result->revision++;return UMI_STATUS_OK;}
