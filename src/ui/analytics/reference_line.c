/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/reference_line.c
 *
 * PURPOSE:
 *   Describe labelled horizontal or vertical analytical reference lines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/reference_line.h"

#include <string.h>
UmiStatus umi_analytics_reference_line_init(UmiAnalyticsReferenceLine *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Reference");item->orientation=UMI_ANALYTICS_HORIZONTAL;return UMI_STATUS_OK;}
int umi_analytics_reference_line_valid(const UmiAnalyticsReferenceLine *item){if(item==NULL)return 0;return (umi_analytics_number_valid(item->value)&&(item->orientation==UMI_ANALYTICS_HORIZONTAL||item->orientation==UMI_ANALYTICS_VERTICAL))?1:0;}
