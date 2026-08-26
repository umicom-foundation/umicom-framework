/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/variant_set.c
 *
 * PURPOSE:
 *   Maintain a bounded unique collection of adaptive design variants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_set.h"
#include <string.h>

/* Reset the bounded adaptive variant collection. */
void umi_designer_adaptive_variant_set_init(UmiDesignerAdaptiveVariantSet *set)
{
    if (set != NULL) memset(set, 0, sizeof *set);
}

/* Add a variant while preventing duplicate identifiers and capacity overflow. */
UmiStatus umi_designer_adaptive_variant_set_add(UmiDesignerAdaptiveVariantSet *set,
                                                const UmiDesignerAdaptiveVariant *variant)
{
    if (set == NULL || variant == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_designer_adaptive_variant_set_find(set, variant->variant_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (set->count >= UMI_DESIGNER_ADAPTIVE_MAX_VARIANTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->variants[set->count++] = *variant;
    return UMI_STATUS_OK;
}

/* Resolve a variant by stable identifier for inheritance and preview selection. */
const UmiDesignerAdaptiveVariant *umi_designer_adaptive_variant_set_find(const UmiDesignerAdaptiveVariantSet *set,
                                                                        const char *variant_id)
{
    size_t index;
    if (set == NULL || variant_id == NULL) return NULL;
    for (index = 0U; index < set->count; ++index)
        if (strcmp(set->variants[index].variant_id, variant_id) == 0) return &set->variants[index];
    return NULL;
}
