/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/correlation_heatmap.c
 *
 * PURPOSE:
 *   Maintain symmetric bounded correlation matrices with unit diagonals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/correlation_heatmap.h"

#include <string.h>
/*
 * Initialise analytics correlation heatmap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_correlation_heatmap_init(UmiAnalyticsCorrelationHeatmap *m,uint16_t size){uint16_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||size==0U||size>UMI_ANALYTICS_MAX_MATRIX)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof *m);m->size=size;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<size;i++)m->values[(size_t)i*(size_t)size+i]=1.0;return UMI_STATUS_OK;}
/*
 * Copy analytics correlation heatmap into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_analytics_correlation_heatmap_set(UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||a>=m->size||b>=m->size||c<-1.0||c>1.0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==b&&c!=1.0)return UMI_STATUS_INVALID_ARGUMENT;m->values[(size_t)a*(size_t)m->size+b]=c;m->values[(size_t)b*(size_t)m->size+a]=c;return UMI_STATUS_OK;}
/*
 * Provide the analytics correlation heatmap get operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_correlation_heatmap_get(const UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double *out_value){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out_value==NULL||a>=m->size||b>=m->size)return UMI_STATUS_INVALID_ARGUMENT;*out_value=m->values[(size_t)a*(size_t)m->size+b];return UMI_STATUS_OK;}
