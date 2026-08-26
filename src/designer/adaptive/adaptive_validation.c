/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_validation.c
 *
 * PURPOSE:
 *   Aggregate blocking adaptive-designer validation issues across variants, rules and preview cases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_validation.h"
#include <stdint.h>

/* Count only issues that would produce ambiguous or incorrect runtime adaptation. */
uint32_t umi_designer_adaptive_validation_blocking_count(const UmiDesignerAdaptiveValidation *validation)
{
    if(validation==NULL)return UINT32_MAX;
    return validation->invalid_variants+validation->invalid_rules+validation->failed_previews;
}
/* Publish only when every blocking adaptive issue has been resolved. */
int umi_designer_adaptive_validation_publishable(const UmiDesignerAdaptiveValidation *validation)
{
    return validation!=NULL&&umi_designer_adaptive_validation_blocking_count(validation)==0U;
}
