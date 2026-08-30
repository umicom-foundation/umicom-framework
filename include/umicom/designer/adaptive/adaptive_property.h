/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/adaptive_property.h
 *
 * PURPOSE:
 *   Describe one responsive property path and its inherited base value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_PROPERTY_H
#define UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_PROPERTY_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveProperty {
    char property_path[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignerAdaptiveValue base_value;
    int inheritable;
} UmiDesignerAdaptiveProperty;
/* Initialise one responsive property descriptor. */
UmiStatus umi_designer_adaptive_property_init(UmiDesignerAdaptiveProperty *property,
                                              const char *property_path,
                                              const char *base_text,
                                              int inheritable);

#ifdef __cplusplus
}
#endif
#endif
