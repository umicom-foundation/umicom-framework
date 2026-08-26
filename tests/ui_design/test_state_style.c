/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_state_style.c
 *
 * PURPOSE:
 *   Verify state-style semantic roles and normalized opacity.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/state_style.h"

int main(void){UmiDesignStateStyle s;return umi_design_state_style_init(&s,UMI_DESIGN_INTERACTION_DISABLED,UMI_DESIGN_COLOR_MUTED,UMI_DESIGN_COLOR_SURFACE,UMI_DESIGN_COLOR_BORDER,0.5)==UMI_STATUS_OK&&s.opacity==0.5?0:1;}
