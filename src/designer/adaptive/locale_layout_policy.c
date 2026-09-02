/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/locale_layout_policy.c
 *
 * PURPOSE:
 *   Model right-to-left mirroring and text-expansion allowances in responsive designer previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/locale_layout_policy.h"
#include <stdint.h>
#include <limits.h>

/* Bound localisation expansion to a practical 0-100 percent design-time allowance. */
UmiStatus umi_designer_locale_layout_policy_init(UmiDesignerLocaleLayoutPolicy *policy,
                                                 int right_to_left,
                                                 int mirror_navigation,
                                                 uint16_t text_expansion_percent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(policy==NULL||text_expansion_percent>100U)return UMI_STATUS_INVALID_ARGUMENT;
    policy->right_to_left=right_to_left!=0;
    policy->mirror_navigation=mirror_navigation!=0;
    policy->text_expansion_percent=text_expansion_percent;
    return UMI_STATUS_OK;
}
/* Apply integer-safe percentage expansion for deterministic preview bounds. */
int32_t umi_designer_locale_layout_policy_expand_width(const UmiDesignerLocaleLayoutPolicy *policy,
                                                       int32_t width)
{
    int64_t expanded;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(policy==NULL||width<0)return -1;
    expanded=(int64_t)width+((int64_t)width*(int64_t)policy->text_expansion_percent)/100;
    return expanded>INT32_MAX?INT32_MAX:(int32_t)expanded;
}
