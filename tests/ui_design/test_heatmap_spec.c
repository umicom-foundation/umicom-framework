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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignHeatmapSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_heatmap_spec_init(&s,10U,12U,-1.0,1.0,1)!=UMI_STATUS_OK)return 1;return s.columns==12U?0:2;}
