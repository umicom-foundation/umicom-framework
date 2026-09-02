/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/category_value.c
 *
 * PURPOSE:
 *   Pair a bounded category label with a finite numeric value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/category_value.h"

/*
 * Initialise analytics category value from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_category_value_init(UmiAnalyticsCategoryValue *item,const char *category,double value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;s=umi_analytics_copy_text(item->category,sizeof item->category,category);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;item->value=value;return UMI_STATUS_OK;}
