/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_layout.c
 *
 * PURPOSE:
 *   Verify compound component layout semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_layout.h"

int main(void){UmiDesignComponentLayout l;if(umi_design_component_layout_init(&l,UMI_DESIGN_LAYOUT_GRID,4U,8.0,1)!=UMI_STATUS_OK)return 1;return l.columns==4U&&l.gap==8.0?0:2;}
