/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_model.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
int main(void){UmiChartModel m;UmiChartSeries s;UmiChartSnapshot snap;assert(umi_chart_model_init(&m,"NQ")==UMI_STATUS_OK);assert(umi_chart_series_init(&s,"close",UMI_CHART_LINE)==UMI_STATUS_OK);assert(umi_chart_series_add(&s,(UmiChartPoint){1,100.0})==UMI_STATUS_OK);assert(umi_chart_model_add_series(&m,&s)==UMI_STATUS_OK);assert(umi_chart_snapshot(&m,&snap)==UMI_STATUS_OK);assert(snap.point_count==1U);return 0;}
