/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/reference_line.c
 *
 * PURPOSE:
 *   Describe labelled horizontal or vertical analytical reference lines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/reference_line.h"

#include <string.h>
/*
 * Initialise analytics reference line from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_reference_line_init(UmiAnalyticsReferenceLine *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Reference");item->orientation=UMI_ANALYTICS_HORIZONTAL;return UMI_STATUS_OK;}
/*
 * Check that analytics reference line satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_reference_line_valid(const UmiAnalyticsReferenceLine *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (umi_analytics_number_valid(item->value)&&(item->orientation==UMI_ANALYTICS_HORIZONTAL||item->orientation==UMI_ANALYTICS_VERTICAL))?1:0;}
