/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis.c
 *
 * PURPOSE:
 *   Describe one semantic chart axis without renderer-specific coordinates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis.h"

#include <string.h>
UmiStatus umi_analytics_axis_init(UmiAnalyticsAxis *axis,const char *id,const char *label,UmiAnalyticsAxisScale scale,double minimum,double maximum){UmiStatus s;if(axis==NULL||minimum>=maximum||!umi_analytics_number_valid(minimum)||!umi_analytics_number_valid(maximum))return UMI_STATUS_INVALID_ARGUMENT;memset(axis,0,sizeof *axis);s=umi_analytics_copy_text(axis->id,sizeof axis->id,id);if(s!=0)return s;s=umi_analytics_copy_text(axis->label,sizeof axis->label,label);if(s!=0)return s;axis->scale=scale;axis->minimum=minimum;axis->maximum=maximum;return UMI_STATUS_OK;}
int umi_analytics_axis_valid(const UmiAnalyticsAxis *axis){return axis!=NULL&&axis->id[0]!='\0'&&axis->scale>=UMI_ANALYTICS_SCALE_LINEAR&&axis->scale<=UMI_ANALYTICS_SCALE_CATEGORY&&axis->minimum<axis->maximum;}
