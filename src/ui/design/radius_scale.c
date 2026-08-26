/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/radius_scale.c
 *
 * PURPOSE:
 *   Define consistent corner radii shared across panels, cards and controls.
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

UmiDesignRadiusScale radius_scale_default(void) { UmiDesignRadiusScale s={{0, 2, 4, 6, 8, 12, 16, 24}}; return s; }
UmiStatus radius_scale_value(const UmiDesignRadiusScale *scale,size_t index,double *out_value) { if(scale==NULL||out_value==NULL||index>=8U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->values[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
