/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/variant_designer.c
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
#include "umicom/designer/adaptive/variant_designer.h"
#include <string.h>

/* Reset the adaptive variant editor. */
void umi_designer_variant_designer_init(UmiDesignerVariantDesigner *designer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(designer!=NULL){memset(designer,0,sizeof *designer); umi_designer_adaptive_variant_set_init(&designer->variants);}
}
/* Insert through the canonical variant set and advance authoring revision. */
UmiStatus umi_designer_variant_designer_add(UmiDesignerVariantDesigner *designer,
                                            const UmiDesignerAdaptiveVariant *variant)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(designer==NULL||variant==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_designer_adaptive_variant_set_add(&designer->variants,variant); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(status!=UMI_STATUS_OK)return status;
    status=umi_designer_adaptive_copy_text(designer->selected_variant_id,sizeof designer->selected_variant_id,variant->variant_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    ++designer->revision;
    return UMI_STATUS_OK;
}
