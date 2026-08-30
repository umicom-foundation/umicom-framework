/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_variant.c
 *
 * PURPOSE:
 *   Verify named component variants retain semantic intent.
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

#include "umicom/ui/design/component_variant.h"

int main(void){UmiDesignComponentVariant v;return umi_design_component_variant_init(&v,"danger-outline",UMI_DESIGN_ROLE_DANGER,1,0)==UMI_STATUS_OK&&v.outlined?0:1;}
