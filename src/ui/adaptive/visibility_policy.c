/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/visibility_policy.c
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
#include "umicom/ui/adaptive/visibility_policy.h"

/* Preserve essential content when configured, otherwise enforce class and priority thresholds. */
int umi_adaptive_visibility_policy_evaluate(const UmiAdaptiveVisibilityPolicy *policy,
                                            UmiDesignSizeClass size_class,
                                            const UmiAdaptiveContentPriority *content)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || content == NULL) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (policy->essential_always_visible != 0 && content->essential != 0) return 1;
    return size_class >= policy->minimum_class && content->priority >= policy->minimum_priority;
}
