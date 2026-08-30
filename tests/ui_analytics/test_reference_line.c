/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_reference_line.c
 *
 * PURPOSE:
 *   Validate reference_line analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/reference_line.h"

int main(void){UmiAnalyticsReferenceLine item;if(umi_analytics_reference_line_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_reference_line_valid(&item))?0:2;}
