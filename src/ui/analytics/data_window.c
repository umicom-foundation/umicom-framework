/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/data_window.c
 *
 * PURPOSE:
 *   Resolve bounded visible index windows for large analytical datasets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_window.h"

UmiStatus umi_analytics_data_window_resolve(size_t total,size_t first,size_t requested,UmiAnalyticsDataWindow *out_window){size_t remaining;if(out_window==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(first>total)first=total;remaining=total-first;out_window->first=first;out_window->count=requested<remaining?requested:remaining;return UMI_STATUS_OK;}
