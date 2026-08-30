/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_heatmap_spec.c
 *
 * PURPOSE:
 *   Verify the semantic heatmap spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/heatmap_spec.h"

int main(void){UmiDesignHeatmapSpec s;if(umi_design_heatmap_spec_init(&s,10U,12U,-1.0,1.0,1)!=UMI_STATUS_OK)return 1;return s.columns==12U?0:2;}
