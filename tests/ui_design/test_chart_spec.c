/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_chart_spec.c
 *
 * PURPOSE:
 *   Verify reusable chart interaction and series semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/chart_spec.h"

int main(void){UmiDesignChartSpec s;if(umi_design_chart_spec_init(&s,UMI_DESIGN_CHART_CANDLESTICK,1U,0,1,1,1)!=UMI_STATUS_OK)return 1;return s.crosshair&&s.zoom?0:2;}
