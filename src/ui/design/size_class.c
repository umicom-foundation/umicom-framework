/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/size_class.c
 *
 * PURPOSE:
 *   Classify available width into stable responsive size classes.
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

#include "umicom/ui/design/size_class.h"

/*
 * Provide the design size class from width operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_size_class_from_width(int32_t width,UmiDesignSizeClass *out_class){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_class==NULL||width<0)return UMI_STATUS_INVALID_ARGUMENT;*out_class=width<600?UMI_DESIGN_SIZE_COMPACT:(width<900?UMI_DESIGN_SIZE_MEDIUM:(width<1440?UMI_DESIGN_SIZE_EXPANDED:UMI_DESIGN_SIZE_WIDE));return UMI_STATUS_OK;}
/*
 * Provide the design size class name operation used by this module and its client
 * applications.
 */
const char *umi_design_size_class_name(UmiDesignSizeClass size_class){/* Select the behaviour associated with the requested command or state value. */ switch(size_class){case UMI_DESIGN_SIZE_COMPACT:return "compact";case UMI_DESIGN_SIZE_MEDIUM:return "medium";case UMI_DESIGN_SIZE_EXPANDED:return "expanded";case UMI_DESIGN_SIZE_WIDE:return "wide";default:return "unknown";}}
