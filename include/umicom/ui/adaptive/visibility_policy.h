/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/visibility_policy.h
 *
 * PURPOSE:
 *   Evaluate semantic visibility policy from size class and content priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_VISIBILITY_POLICY_H
#define UMICOM_UI_ADAPTIVE_VISIBILITY_POLICY_H
#include "umicom/ui/adaptive/content_priority.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive visibility policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAdaptiveVisibilityPolicy {
    UmiDesignSizeClass minimum_class;
    uint32_t minimum_priority;
    int essential_always_visible;
} UmiAdaptiveVisibilityPolicy;
/* Evaluate whether content remains visible under the current size and priority constraints. */
int umi_adaptive_visibility_policy_evaluate(const UmiAdaptiveVisibilityPolicy *policy,
                                            UmiDesignSizeClass size_class,
                                            const UmiAdaptiveContentPriority *content);

#ifdef __cplusplus
}
#endif
#endif
