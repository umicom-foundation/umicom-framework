/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/indicator.c
 *
 * PURPOSE:
 *   Compute reusable moving-average indicators over chart series.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/indicator.h"
/*
 * Provide the chart indicator sma operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_indicator_sma(const UmiChartSeries *in,size_t p,UmiChartSeries *out){size_t i,j;double sum;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in==NULL||out==NULL||p==0U)return UMI_STATUS_INVALID_ARGUMENT;s=umi_chart_series_init(out,"sma",UMI_CHART_LINE);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Visit each bounded item once so every record receives the same rule. */ for(i=p-1U;i<in->point_count;++i){sum=0.0;/* Visit each bounded item once so every record receives the same rule. */ for(j=i+1U-p;j<=i;++j)sum+=in->points[j].value;s=umi_chart_series_add(out,(UmiChartPoint){in->points[i].time_ms,sum/(double)p});/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
/*
 * Provide the chart indicator ema operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_indicator_ema(const UmiChartSeries *in,size_t p,UmiChartSeries *out){size_t i;double alpha,ema;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(in==NULL||out==NULL||p==0U||in->point_count==0U)return UMI_STATUS_INVALID_ARGUMENT;s=umi_chart_series_init(out,"ema",UMI_CHART_LINE);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;alpha=2.0/((double)p+1.0);ema=in->points[0].value;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<in->point_count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>0U)ema=alpha*in->points[i].value+(1.0-alpha)*ema;s=umi_chart_series_add(out,(UmiChartPoint){in->points[i].time_ms,ema});/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}return UMI_STATUS_OK;}
