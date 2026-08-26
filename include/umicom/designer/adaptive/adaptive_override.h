/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/adaptive_override.h
 *
 * PURPOSE:
 *   Represent one variant-scoped responsive property override with explicit inheritance mode.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_OVERRIDE_H
#define UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_OVERRIDE_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveOverride {
    char variant_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    char property_path[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignerAdaptiveValue value;
    UmiDesignerAdaptiveInheritance inheritance;
    int active;
} UmiDesignerAdaptiveOverride;
/* Initialise one responsive property override. */
UmiStatus umi_designer_adaptive_override_init(UmiDesignerAdaptiveOverride *override_value,
                                              const char *variant_id,
                                              const char *property_path,
                                              const char *text,
                                              UmiDesignerAdaptiveInheritance inheritance);

#ifdef __cplusplus
}
#endif
#endif
