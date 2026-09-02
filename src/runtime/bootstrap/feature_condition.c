/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/feature_condition.c
 *
 * PURPOSE:
 *   Implement the feature condition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/feature_condition.c
 *
 * PURPOSE:
 *   Evaluate feature-pack presence without application-specific branches.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/feature_condition.h"


#include <string.h>
/*
 * Provide the bootstrap feature condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_feature_condition_match(const UmiBootstrapIdList *features,
                                           const char *feature_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (features == NULL || feature_id == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < features->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(features->ids[i], feature_id) == 0) return true;
    return false;
}
