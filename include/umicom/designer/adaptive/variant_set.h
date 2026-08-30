/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/variant_set.h
 *
 * PURPOSE:
 *   Maintain a bounded unique collection of adaptive design variants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_VARIANT_SET_H
#define UMICOM_DESIGNER_ADAPTIVE_VARIANT_SET_H
#include "umicom/designer/adaptive/adaptive_variant.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveVariantSet {
    UmiDesignerAdaptiveVariant variants[UMI_DESIGNER_ADAPTIVE_MAX_VARIANTS];
    size_t count;
} UmiDesignerAdaptiveVariantSet;
/* Initialise an empty variant set. */
void umi_designer_adaptive_variant_set_init(UmiDesignerAdaptiveVariantSet *set);
/* Add one uniquely identified adaptive variant. */
UmiStatus umi_designer_adaptive_variant_set_add(UmiDesignerAdaptiveVariantSet *set,
                                                const UmiDesignerAdaptiveVariant *variant);
/* Find one variant by stable variant identifier. */
const UmiDesignerAdaptiveVariant *umi_designer_adaptive_variant_set_find(const UmiDesignerAdaptiveVariantSet *set,
                                                                        const char *variant_id);

#ifdef __cplusplus
}
#endif
#endif
