/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/elevation_scale.c
 *
 * PURPOSE:
 *   Define renderer-neutral elevation levels for surfaces and transient overlays.
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

UmiDesignElevationScale elevation_scale_default(void) { UmiDesignElevationScale s={{0, 1, 2, 4, 8, 16}}; return s; }
UmiStatus elevation_scale_value(const UmiDesignElevationScale *scale,size_t index,double *out_value) { if(scale==NULL||out_value==NULL||index>=6U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->depth[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
