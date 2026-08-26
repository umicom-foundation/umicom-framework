/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/variant_resolver.c
 *
 * PURPOSE:
 *   Choose the highest-priority adaptive variant matching size class and orientation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_resolver.h"

/* Select the highest-priority exact class/orientation match deterministically. */
UmiStatus umi_designer_variant_resolver_resolve(const UmiDesignerAdaptiveVariantSet *set,
                                                UmiDesignSizeClass size_class,
                                                UmiAdaptiveDisplayOrientation orientation,
                                                UmiDesignerAdaptiveVariant *out_variant)
{
    size_t index;
    const UmiDesignerAdaptiveVariant *best = NULL;
    if (set == NULL || out_variant == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < set->count; ++index) {
        const UmiDesignerAdaptiveVariant *candidate = &set->variants[index];
        if (candidate->size_class == size_class && candidate->orientation == orientation &&
            (best == NULL || candidate->priority > best->priority)) best = candidate;
    }
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_variant = *best;
    return UMI_STATUS_OK;
}
