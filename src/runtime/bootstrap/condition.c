/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/condition.c
 *
 * PURPOSE:
 *   Implement the condition behavior for
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
 * File: src/runtime/bootstrap/condition.c
 *
 * PURPOSE:
 *   Describe one property, feature, capability, platform or environment condition.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/condition.h"
#include "../../base/value_construction_internal.h"


#include <string.h>
/*
 * Initialise bootstrap condition from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_condition_init(UmiBootstrapCondition *condition,
                                       UmiBootstrapConditionKind kind,
                                       const char *key,
                                       const char *expected,
                                       bool negate) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || !umi_bootstrap_id_valid(key) ||
        kind < UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT ||
        kind > UMI_BOOTSTRAP_CONDITION_ENVIRONMENT_EQUALS)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(condition, 0, sizeof(*condition));
    condition->kind = kind;
    status = umi_bootstrap_copy_text(condition->key, sizeof(condition->key), key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (expected != NULL) {
        status = umi_bootstrap_copy_text(condition->expected,
            sizeof(condition->expected), expected);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    condition->negate = negate;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_condition_init_build_value(UmiBootstrapCondition *condition,
                                       UmiBootstrapConditionKind kind,
                                       const char *key,
                                       const char *expected,
                                       bool negate) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || !umi_bootstrap_id_valid(key) ||
        kind < UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT ||
        kind > UMI_BOOTSTRAP_CONDITION_ENVIRONMENT_EQUALS)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(condition, 0, sizeof(*condition));
    condition->kind = kind;
    status = umi_bootstrap_copy_text(condition->key, sizeof(condition->key), key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (expected != NULL) {
        status = umi_bootstrap_copy_text(condition->expected,
            sizeof(condition->expected), expected);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    condition->negate = negate;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_condition_init, UmiBootstrapCondition, condition,
    (UmiBootstrapCondition *condition, UmiBootstrapConditionKind kind, const char *key, const char *expected, bool negate),
    umi_bootstrap_condition_init_build_value(candidate, kind, key, expected, negate))
