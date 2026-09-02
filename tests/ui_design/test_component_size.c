/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_size.c
 *
 * PURPOSE:
 *   Verify adaptive component-size constraint ordering.
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

#include "umicom/ui/design/component_size.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignComponentSize s;UmiUiSize a={24,24},b={120,32},c={500,80};return umi_design_component_size_init(&s,a,b,c)==UMI_STATUS_OK?0:1;}
