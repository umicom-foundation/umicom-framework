/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_property.c
 *
 * PURPOSE:
 *   Verify typed component-property metadata.
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

#include "umicom/ui/design/component_property.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignComponentProperty p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_property_init(&p,"padding",UMI_DESIGN_VALUE_LENGTH,0,"space.2")!=UMI_STATUS_OK)return 1;return p.value_kind==UMI_DESIGN_VALUE_LENGTH?0:2;}
