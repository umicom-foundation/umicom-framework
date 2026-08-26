/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/kpi_trend.c
 *
 * PURPOSE:
 *   Estimate linear KPI trend slope over ordered observations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_trend.h"

UmiStatus umi_analytics_kpi_trend_slope(const double *values,size_t count,double *out_slope){size_t i;double sx=0,sy=0,sxy=0,sxx=0,n=(double)count,den;if(values==NULL||out_slope==NULL||count<2U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++){double x=(double)i,y=values[i];if(!umi_analytics_number_valid(y))return UMI_STATUS_INVALID_ARGUMENT;sx+=x;sy+=y;sxy+=x*y;sxx+=x*x;}den=n*sxx-sx*sx;if(den==0.0)return UMI_STATUS_INVALID_STATE;*out_slope=(n*sxy-sx*sy)/den;return UMI_STATUS_OK;}
