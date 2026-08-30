/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_spacing_scale.c
 *
 * PURPOSE:
 *   Verify canonical spacing scale scale lookup.
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

#include "umicom/ui/design/spacing_scale.h"

int main(void){UmiDesignSpacingScale s=spacing_scale_default();double v=0.0;if(spacing_scale_value(&s,9U,&v)!=UMI_STATUS_OK)return 1;return v==48?0:2;}
