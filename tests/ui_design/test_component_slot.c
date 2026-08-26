/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_slot.c
 *
 * PURPOSE:
 *   Verify compound-component slot cardinality.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_slot.h"

int main(void){UmiDesignComponentSlot s;if(umi_design_component_slot_init(&s,"actions",UMI_UI_COMPONENT_BUTTON,0U,3U)!=UMI_STATUS_OK)return 1;return umi_design_component_slot_accepts_count(&s,2U)?0:2;}
