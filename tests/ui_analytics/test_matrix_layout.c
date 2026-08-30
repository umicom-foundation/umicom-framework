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

int main(void){UmiAnalyticsMatrixRect r;if(umi_analytics_matrix_layout_cell(2,2,1,1,100,100,0,&r)!=0)return 1;return r.x==50.0&&r.y==50.0?0:2;}
