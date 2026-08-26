/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/variant_designer.c
 *
 * PURPOSE:
 *   Own adaptive-variant selection and insertion for the visual application designer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/variant_designer.h"
#include <string.h>

/* Reset the adaptive variant editor. */
void umi_designer_variant_designer_init(UmiDesignerVariantDesigner *designer)
{
    if(designer!=NULL){memset(designer,0,sizeof *designer); umi_designer_adaptive_variant_set_init(&designer->variants);}
}
/* Insert through the canonical variant set and advance authoring revision. */
UmiStatus umi_designer_variant_designer_add(UmiDesignerVariantDesigner *designer,
                                            const UmiDesignerAdaptiveVariant *variant)
{
    UmiStatus status;
    if(designer==NULL||variant==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_designer_adaptive_variant_set_add(&designer->variants,variant); if(status!=UMI_STATUS_OK)return status;
    status=umi_designer_adaptive_copy_text(designer->selected_variant_id,sizeof designer->selected_variant_id,variant->variant_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    ++designer->revision;
    return UMI_STATUS_OK;
}
