/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_filter.c
 *
 * PURPOSE:
 *   Evaluate reusable numeric analytics filter predicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_filter.h"

/*
 * Initialise analytics filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_filter_init(UmiAnalyticsFilter *f,UmiAnalyticsCompare comparison,double threshold){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL||comparison<UMI_ANALYTICS_COMPARE_GT||comparison>UMI_ANALYTICS_COMPARE_NE||!umi_analytics_number_valid(threshold))return UMI_STATUS_INVALID_ARGUMENT;f->comparison=comparison;f->threshold=threshold;return UMI_STATUS_OK;}
/*
 * Provide the analytics filter matches operation used by this module and its client
 * applications.
 */
int umi_analytics_filter_matches(const UmiAnalyticsFilter *f,double value){return f!=NULL?umi_analytics_compare(value,f->comparison,f->threshold):0;}
