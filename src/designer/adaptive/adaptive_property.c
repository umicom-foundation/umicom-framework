/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_property.c
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
#include "umicom/designer/adaptive/adaptive_property.h"
#include <string.h>

/* Capture a base property value that later variants may inherit or override. */
UmiStatus umi_designer_adaptive_property_init(UmiDesignerAdaptiveProperty *property,
                                              const char *property_path,
                                              const char *base_text,
                                              int inheritable)
{
    UmiStatus status;
    if (property == NULL || property_path == NULL || base_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(property, 0, sizeof *property);
    status = umi_designer_adaptive_copy_text(property->property_path, sizeof property->property_path, property_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(property->base_value.text, sizeof property->base_value.text, base_text);
    if (status != UMI_STATUS_OK) return status;
    property->inheritable = inheritable != 0;
    return UMI_STATUS_OK;
}
