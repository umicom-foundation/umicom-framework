/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/motion_scale.c
 *
 * PURPOSE:
 *   Define standard animation durations for feedback, transitions and workspace motion.
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

UmiDesignMotionScale motion_scale_default(void) { UmiDesignMotionScale s={{0, 80, 120, 180, 240, 320}}; return s; }
UmiStatus motion_scale_value(const UmiDesignMotionScale *scale,size_t index,double *out_value) { if(scale==NULL||out_value==NULL||index>=6U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->milliseconds[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
