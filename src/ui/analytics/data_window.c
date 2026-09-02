/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/data_window.c
 *
 * PURPOSE:
 *   Resolve bounded visible index windows for large analytical datasets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_window.h"

/*
 * Provide the analytics data window resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_data_window_resolve(size_t total,size_t first,size_t requested,UmiAnalyticsDataWindow *out_window){size_t remaining;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_window==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(first>total)first=total;remaining=total-first;out_window->first=first;out_window->count=requested<remaining?requested:remaining;return UMI_STATUS_OK;}
