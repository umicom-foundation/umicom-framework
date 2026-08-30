/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/size_class.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_SIZE_CLASS_H
#define INCLUDE_UMICOM_UI_DESIGN_SIZE_CLASS_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignSizeClass { UMI_DESIGN_SIZE_COMPACT=1, UMI_DESIGN_SIZE_MEDIUM=2, UMI_DESIGN_SIZE_EXPANDED=3, UMI_DESIGN_SIZE_WIDE=4 } UmiDesignSizeClass;
/* Classify a non-negative viewport width using Framework baseline thresholds. */
UmiStatus umi_design_size_class_from_width(int32_t width, UmiDesignSizeClass *out_class);
/* Return a stable name for a responsive size class. */
const char *umi_design_size_class_name(UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif

#endif
