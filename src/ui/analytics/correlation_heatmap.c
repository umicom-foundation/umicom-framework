/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/correlation_heatmap.c
 *
 * PURPOSE:
 *   Maintain symmetric bounded correlation matrices with unit diagonals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/correlation_heatmap.h"

#include <string.h>
UmiStatus umi_analytics_correlation_heatmap_init(UmiAnalyticsCorrelationHeatmap *m,uint16_t size){uint16_t i;if(m==NULL||size==0U||size>UMI_ANALYTICS_MAX_MATRIX)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof *m);m->size=size;for(i=0;i<size;i++)m->values[(size_t)i*(size_t)size+i]=1.0;return UMI_STATUS_OK;}
UmiStatus umi_analytics_correlation_heatmap_set(UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double c){if(m==NULL||a>=m->size||b>=m->size||c<-1.0||c>1.0)return UMI_STATUS_INVALID_ARGUMENT;if(a==b&&c!=1.0)return UMI_STATUS_INVALID_ARGUMENT;m->values[(size_t)a*(size_t)m->size+b]=c;m->values[(size_t)b*(size_t)m->size+a]=c;return UMI_STATUS_OK;}
UmiStatus umi_analytics_correlation_heatmap_get(const UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double *out_value){if(m==NULL||out_value==NULL||a>=m->size||b>=m->size)return UMI_STATUS_INVALID_ARGUMENT;*out_value=m->values[(size_t)a*(size_t)m->size+b];return UMI_STATUS_OK;}
