/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_indicator.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiChartSeries in,out;assert(umi_chart_series_init(&in,"close",UMI_CHART_LINE)==UMI_STATUS_OK);/* Visit each bounded item once so every record receives the same rule. */ for(int i=0;i<5;++i)assert(umi_chart_series_add(&in,(UmiChartPoint){i,(double)(i+1)})==UMI_STATUS_OK);assert(umi_chart_indicator_sma(&in,3U,&out)==UMI_STATUS_OK);assert(out.point_count==3U);assert(out.points[0].value==2.0);return 0;}
