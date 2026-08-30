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
void umi_analytics_result_init(UmiAnalyticsResult *result){if(result!=NULL)memset(result,0,sizeof *result);}
UmiStatus umi_analytics_result_add(UmiAnalyticsResult *result,const char *key,double value){UmiStatus s;if(result==NULL||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;if(result->count>=UMI_ANALYTICS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_analytics_copy_text(result->rows[result->count].key,sizeof result->rows[result->count].key,key);if(s!=0)return s;result->rows[result->count].value=value;result->count++;result->revision++;return UMI_STATUS_OK;}
