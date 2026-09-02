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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/motion_scale.h"

/*
 * Provide the motion scale default operation used by this module and its client
 * applications.
 */
UmiDesignMotionScale motion_scale_default(void) { UmiDesignMotionScale s={{0, 80, 120, 180, 240, 320}}; return s; }
/*
 * Provide the motion scale value operation used by this module and its client
 * applications.
 */
UmiStatus motion_scale_value(const UmiDesignMotionScale *scale,size_t index,double *out_value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale==NULL||out_value==NULL||index>=6U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->milliseconds[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
