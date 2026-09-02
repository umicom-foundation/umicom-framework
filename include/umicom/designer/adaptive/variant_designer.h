/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/variant_designer.h
 *
 * PURPOSE:
 *   Own adaptive-variant selection and insertion for the visual application designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_VARIANT_DESIGNER_H
#define UMICOM_DESIGNER_ADAPTIVE_VARIANT_DESIGNER_H
#include "umicom/designer/adaptive/variant_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer variant designer data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerVariantDesigner {
    UmiDesignerAdaptiveVariantSet variants;
    char selected_variant_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    uint64_t revision;
} UmiDesignerVariantDesigner;
/* Initialise an empty variant-designer session. */
void umi_designer_variant_designer_init(UmiDesignerVariantDesigner *designer);
/* Add and select one adaptive variant. */
UmiStatus umi_designer_variant_designer_add(UmiDesignerVariantDesigner *designer,
                                            const UmiDesignerAdaptiveVariant *variant);

#ifdef __cplusplus
}
#endif
#endif
