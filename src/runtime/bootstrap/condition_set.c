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
/*
 * Initialise bootstrap condition set from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_condition_set_init(UmiBootstrapConditionSet *set,
                                      UmiBootstrapConditionMode mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return;
    memset(set, 0, sizeof(*set));
    set->mode = mode;
}
/*
 * Add bootstrap condition set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_condition_set_add(UmiBootstrapConditionSet *set,
                                          const UmiBootstrapCondition *condition) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || condition == NULL || !umi_bootstrap_id_valid(condition->key))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *condition;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap condition set reduce operation used by this module and its client
 * applications.
 */
bool umi_bootstrap_condition_set_reduce(const bool *results,
                                        size_t count,
                                        UmiBootstrapConditionMode mode) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL && count > 0U) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mode == UMI_BOOTSTRAP_CONDITION_ANY) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (i = 0U; i < count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (results[i]) return true;
        return count == 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!results[i]) return false;
    return true;
}
