/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/opacity_scale.c
 *
 * PURPOSE:
 *   Define common opacity levels for disabled, muted and overlay presentation.
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

/*
 * Provide the opacity scale default operation used by this module and its client
 * applications.
 */
UmiDesignOpacityScale opacity_scale_default(void) { UmiDesignOpacityScale s={{0, 0.12, 0.24, 0.38, 0.56, 0.72, 0.88, 1}}; return s; }
/*
 * Provide the opacity scale value operation used by this module and its client
 * applications.
 */
UmiStatus opacity_scale_value(const UmiDesignOpacityScale *scale,size_t index,double *out_value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale==NULL||out_value==NULL||index>=8U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->values[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
