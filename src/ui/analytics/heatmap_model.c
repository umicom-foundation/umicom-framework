/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/heatmap_model.c
 *
 * PURPOSE:
 *   Own a bounded numeric matrix while reusing canonical Design System heatmap specifications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_model.h"

#include <string.h>
static size_t heat_index(const UmiAnalyticsHeatmapModel *m,uint16_t row,uint16_t column){return (size_t)row*(size_t)m->spec.columns+(size_t)column;}
UmiStatus umi_analytics_heatmap_model_init(UmiAnalyticsHeatmapModel *m,const char *id,uint16_t rows,uint16_t columns,double minimum,double maximum){UmiStatus s;if(m==NULL||rows==0U||columns==0U||rows>UMI_ANALYTICS_MAX_MATRIX||columns>UMI_ANALYTICS_MAX_MATRIX||minimum>=maximum)return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof *m);s=umi_analytics_copy_text(m->id,sizeof m->id,id);if(s!=0)return s;m->spec.rows=rows;m->spec.columns=columns;m->spec.minimum=minimum;m->spec.maximum=maximum;m->spec.show_labels=1;return UMI_STATUS_OK;}
UmiStatus umi_analytics_heatmap_model_set(UmiAnalyticsHeatmapModel *m,uint16_t row,uint16_t column,double value){size_t idx;if(m==NULL||row>=m->spec.rows||column>=m->spec.columns||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;idx=heat_index(m,row,column);m->values[idx]=value;m->present[idx]=1U;return UMI_STATUS_OK;}
UmiStatus umi_analytics_heatmap_model_get(const UmiAnalyticsHeatmapModel *m,uint16_t row,uint16_t column,double *out_value){size_t idx;if(m==NULL||out_value==NULL||row>=m->spec.rows||column>=m->spec.columns)return UMI_STATUS_INVALID_ARGUMENT;idx=heat_index(m,row,column);if(m->present[idx]==0U)return UMI_STATUS_NOT_FOUND;*out_value=m->values[idx];return UMI_STATUS_OK;}
