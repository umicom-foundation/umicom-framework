/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_event.c
 *
 * PURPOSE:
 *   Verify semantic event binding and stable naming.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_event.h"

#include <string.h>
int main(void){UmiDesignComponentEvent e;if(umi_design_component_event_init(&e,UMI_DESIGN_EVENT_SUBMIT,"form.submit")!=UMI_STATUS_OK)return 1;return strcmp(umi_design_component_event_name(e.kind),"submit")==0?0:2;}
