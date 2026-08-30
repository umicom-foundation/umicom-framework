/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_opacity_scale.c
 *
 * PURPOSE:
 *   Verify canonical opacity scale scale lookup.
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

#include "umicom/ui/design/opacity_scale.h"

int main(void){UmiDesignOpacityScale s=opacity_scale_default();double v=0.0;if(opacity_scale_value(&s,7U,&v)!=UMI_STATUS_OK)return 1;return v==1?0:2;}
