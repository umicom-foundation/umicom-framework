/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_motion_scale.c
 *
 * PURPOSE:
 *   Verify canonical motion scale scale lookup.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/motion_scale.h"

int main(void){UmiDesignMotionScale s=motion_scale_default();double v=0.0;if(motion_scale_value(&s,5U,&v)!=UMI_STATUS_OK)return 1;return v==320?0:2;}
