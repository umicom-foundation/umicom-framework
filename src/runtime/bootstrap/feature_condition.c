/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/feature_condition.c
 *
 * PURPOSE:
 *   Evaluate feature-pack presence without application-specific branches.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/feature_condition.h"


#include <string.h>
bool umi_bootstrap_feature_condition_match(const UmiBootstrapIdList *features,
                                           const char *feature_id) {
    size_t i;
    if (features == NULL || feature_id == NULL) return false;
    for (i = 0U; i < features->count; ++i)
        if (strcmp(features->ids[i], feature_id) == 0) return true;
    return false;
}
