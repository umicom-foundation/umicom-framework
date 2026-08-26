/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/locale_layout_policy.h
 *
 * PURPOSE:
 *   Model right-to-left mirroring and text-expansion allowances in responsive designer previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_LOCALE_LAYOUT_POLICY_H
#define UMICOM_DESIGNER_ADAPTIVE_LOCALE_LAYOUT_POLICY_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerLocaleLayoutPolicy {
    int right_to_left;
    int mirror_navigation;
    uint16_t text_expansion_percent;
} UmiDesignerLocaleLayoutPolicy;
/* Initialise locale-aware layout policy for preview and conformance checks. */
UmiStatus umi_designer_locale_layout_policy_init(UmiDesignerLocaleLayoutPolicy *policy,
                                                 int right_to_left,
                                                 int mirror_navigation,
                                                 uint16_t text_expansion_percent);
/* Expand an authored logical width by the configured localisation allowance. */
int32_t umi_designer_locale_layout_policy_expand_width(const UmiDesignerLocaleLayoutPolicy *policy,
                                                       int32_t width);

#ifdef __cplusplus
}
#endif
#endif
