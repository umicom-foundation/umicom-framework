/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/condition_set.c
 *
 * PURPOSE:
 *   Implement the condition set behavior for
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
 * File: src/runtime/bootstrap/condition_set.c
 *
 * PURPOSE:
 *   Combine conditions using explicit ALL or ANY semantics.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/condition_set.h"


#include <string.h>
void umi_bootstrap_condition_set_init(UmiBootstrapConditionSet *set,
                                      UmiBootstrapConditionMode mode) {
    if (set == NULL) return;
    memset(set, 0, sizeof(*set));
    set->mode = mode;
}
UmiStatus umi_bootstrap_condition_set_add(UmiBootstrapConditionSet *set,
                                          const UmiBootstrapCondition *condition) {
    if (set == NULL || condition == NULL || !umi_bootstrap_id_valid(condition->key))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (set->count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *condition;
    return UMI_STATUS_OK;
}
bool umi_bootstrap_condition_set_reduce(const bool *results,
                                        size_t count,
                                        UmiBootstrapConditionMode mode) {
    size_t i;
    if (results == NULL && count > 0U) return false;
    if (mode == UMI_BOOTSTRAP_CONDITION_ANY) {
        for (i = 0U; i < count; ++i) if (results[i]) return true;
        return count == 0U;
    }
    for (i = 0U; i < count; ++i) if (!results[i]) return false;
    return true;
}
