/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_override.c
 *
 * PURPOSE:
 *   Represent one variant-scoped responsive property override with explicit inheritance mode.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_override.h"
#include <string.h>

/* Store variant-specific state without modifying the canonical base property. */
UmiStatus umi_designer_adaptive_override_init(UmiDesignerAdaptiveOverride *override_value,
                                              const char *variant_id,
                                              const char *property_path,
                                              const char *text,
                                              UmiDesignerAdaptiveInheritance inheritance)
{
    UmiStatus status;
    if (override_value == NULL || variant_id == NULL || property_path == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(override_value, 0, sizeof *override_value);
    status = umi_designer_adaptive_copy_text(override_value->variant_id, sizeof override_value->variant_id, variant_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(override_value->property_path, sizeof override_value->property_path, property_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(override_value->value.text, sizeof override_value->value.text, text);
    if (status != UMI_STATUS_OK) return status;
    override_value->inheritance = inheritance;
    override_value->active = 1;
    return UMI_STATUS_OK;
}
