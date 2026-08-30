/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_variant.c
 *
 * PURPOSE:
 *   Define one responsive design variant scoped by breakpoint and orientation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_variant.h"
#include <string.h>

/* Define one variant and preserve parent identity for later inheritance resolution. */
UmiStatus umi_designer_adaptive_variant_init(UmiDesignerAdaptiveVariant *variant,
                                             const char *variant_id,
                                             const char *parent_id,
                                             const char *breakpoint_id,
                                             UmiDesignSizeClass size_class,
                                             UmiAdaptiveDisplayOrientation orientation,
                                             uint32_t priority)
{
    UmiStatus status;
    if (variant == NULL || variant_id == NULL || breakpoint_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(variant, 0, sizeof *variant);
    status = umi_designer_adaptive_copy_text(variant->variant_id, sizeof variant->variant_id, variant_id);
    if (status != UMI_STATUS_OK) return status;
    if (parent_id != NULL && parent_id[0] != '\0') {
        status = umi_designer_adaptive_copy_text(variant->parent_id, sizeof variant->parent_id, parent_id);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_designer_adaptive_copy_text(variant->breakpoint_id, sizeof variant->breakpoint_id, breakpoint_id);
    if (status != UMI_STATUS_OK) return status;
    variant->size_class = size_class;
    variant->orientation = orientation;
    variant->priority = priority;
    return UMI_STATUS_OK;
}
