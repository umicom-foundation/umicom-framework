/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_capability.c
 *
 * PURPOSE:
 *   Verify component capability composition.
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

#include "umicom/ui/design/component_capability.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignComponentCapability c={0};umi_design_component_capability_add(&c,UMI_DESIGN_CAP_KEYBOARD);umi_design_component_capability_add(&c,UMI_DESIGN_CAP_FOCUS);return umi_design_component_capability_has(&c,UMI_DESIGN_CAP_KEYBOARD)?0:1;}
