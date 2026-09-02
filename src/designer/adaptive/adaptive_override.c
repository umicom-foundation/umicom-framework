/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_override.c
 *
 * PURPOSE:
 *   Represent one variant-scoped responsive property override with explicit inheritance mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (override_value == NULL || variant_id == NULL || property_path == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(override_value, 0, sizeof *override_value);
    status = umi_designer_adaptive_copy_text(override_value->variant_id, sizeof override_value->variant_id, variant_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(override_value->property_path, sizeof override_value->property_path, property_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_designer_adaptive_copy_text(override_value->value.text, sizeof override_value->value.text, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    override_value->inheritance = inheritance;
    override_value->active = 1;
    return UMI_STATUS_OK;
}
