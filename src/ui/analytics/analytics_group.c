/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_group.c
 *
 * PURPOSE:
 *   Accumulate named group count and finite sum for analytical grouping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_group.h"

#include <string.h>
/*
 * Initialise analytics group from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_group_init(UmiAnalyticsGroup *g,const char *key){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(g,0,sizeof *g);return umi_analytics_copy_text(g->key,sizeof g->key,key);}
/* Add analytics group only after its inputs and available capacity have been checked. */
UmiStatus umi_analytics_group_add(UmiAnalyticsGroup *g,double value){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;g->count++;g->sum+=value;return UMI_STATUS_OK;}
