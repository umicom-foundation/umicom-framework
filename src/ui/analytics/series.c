/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/series.c
 *
 * PURPOSE:
 *   Own a bounded ordered collection of Cartesian analytics samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/series.h"

#include <string.h>
/*
 * Initialise analytics series from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_series_init(UmiAnalyticsSeries *series,const char *id,const char *label){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(series==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(series,0,sizeof *series);s=umi_analytics_copy_text(series->id,sizeof series->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;return umi_analytics_copy_text(series->label,sizeof series->label,label);}
/* Add analytics series only after its inputs and available capacity have been checked. */
UmiStatus umi_analytics_series_append(UmiAnalyticsSeries *series,double x,double y){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(series==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(series->count>=UMI_ANALYTICS_MAX_POINTS)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_analytics_data_point_init(&series->points[series->count],x,y);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;series->count++;series->revision++;return UMI_STATUS_OK;}
/*
 * Find analytics series while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_analytics_series_at(const UmiAnalyticsSeries *series,size_t index,UmiAnalyticsDataPoint *out_point){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(series==NULL||out_point==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=series->count)return UMI_STATUS_NOT_FOUND;*out_point=series->points[index];return UMI_STATUS_OK;}
