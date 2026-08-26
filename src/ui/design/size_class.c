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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/size_class.h"

UmiStatus umi_design_size_class_from_width(int32_t width,UmiDesignSizeClass *out_class){if(out_class==NULL||width<0)return UMI_STATUS_INVALID_ARGUMENT;*out_class=width<600?UMI_DESIGN_SIZE_COMPACT:(width<900?UMI_DESIGN_SIZE_MEDIUM:(width<1440?UMI_DESIGN_SIZE_EXPANDED:UMI_DESIGN_SIZE_WIDE));return UMI_STATUS_OK;}
const char *umi_design_size_class_name(UmiDesignSizeClass size_class){switch(size_class){case UMI_DESIGN_SIZE_COMPACT:return "compact";case UMI_DESIGN_SIZE_MEDIUM:return "medium";case UMI_DESIGN_SIZE_EXPANDED:return "expanded";case UMI_DESIGN_SIZE_WIDE:return "wide";default:return "unknown";}}
