/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_elevation_scale.c
 *
 * PURPOSE:
 *   Verify canonical elevation scale scale lookup.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/elevation_scale.h"

int main(void){UmiDesignElevationScale s=elevation_scale_default();double v=0.0;if(elevation_scale_value(&s,5U,&v)!=UMI_STATUS_OK)return 1;return v==16?0:2;}
