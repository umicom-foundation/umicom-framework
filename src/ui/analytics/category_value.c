/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/category_value.c
 *
 * PURPOSE:
 *   Pair a bounded category label with a finite numeric value.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/category_value.h"

UmiStatus umi_analytics_category_value_init(UmiAnalyticsCategoryValue *item,const char *category,double value){UmiStatus s;if(item==NULL||!umi_analytics_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;s=umi_analytics_copy_text(item->category,sizeof item->category,category);if(s!=UMI_STATUS_OK)return s;item->value=value;return UMI_STATUS_OK;}
