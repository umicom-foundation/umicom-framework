/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_radius_scale.c
 *
 * PURPOSE:
 *   Verify canonical radius scale scale lookup.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/radius_scale.h"

int main(void){UmiDesignRadiusScale s=radius_scale_default();double v=0.0;if(radius_scale_value(&s,7U,&v)!=UMI_STATUS_OK)return 1;return v==24?0:2;}
