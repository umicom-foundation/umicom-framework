/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_matrix_layout.c
 *
 * PURPOSE:
 *   Validate matrix_layout analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/matrix_layout.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsMatrixRect r;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_matrix_layout_cell(2,2,1,1,100,100,0,&r)!=0)return 1;return r.x==50.0&&r.y==50.0?0:2;}
