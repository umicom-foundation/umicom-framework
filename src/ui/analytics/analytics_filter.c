/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_filter.c
 *
 * PURPOSE:
 *   Evaluate reusable numeric analytics filter predicates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_filter.h"

UmiStatus umi_analytics_filter_init(UmiAnalyticsFilter *f,UmiAnalyticsCompare comparison,double threshold){if(f==NULL||comparison<UMI_ANALYTICS_COMPARE_GT||comparison>UMI_ANALYTICS_COMPARE_NE||!umi_analytics_number_valid(threshold))return UMI_STATUS_INVALID_ARGUMENT;f->comparison=comparison;f->threshold=threshold;return UMI_STATUS_OK;}
int umi_analytics_filter_matches(const UmiAnalyticsFilter *f,double value){return f!=NULL?umi_analytics_compare(value,f->comparison,f->threshold):0;}
