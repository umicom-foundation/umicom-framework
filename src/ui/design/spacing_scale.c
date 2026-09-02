/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/spacing_scale.c
 *
 * PURPOSE:
 *   Define consistent spatial rhythm for component padding, gaps and layout regions.
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

/*
 * Provide the spacing scale default operation used by this module and its client
 * applications.
 */
UmiDesignSpacingScale spacing_scale_default(void) { UmiDesignSpacingScale s={{0, 2, 4, 6, 8, 12, 16, 24, 32, 48}}; return s; }
/*
 * Provide the spacing scale value operation used by this module and its client
 * applications.
 */
UmiStatus spacing_scale_value(const UmiDesignSpacingScale *scale,size_t index,double *out_value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(scale==NULL||out_value==NULL||index>=10U)return UMI_STATUS_INVALID_ARGUMENT; *out_value=scale->values[index]; return umi_design_number_valid(*out_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
