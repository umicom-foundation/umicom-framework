/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/condition_evaluator.c
 *
 * PURPOSE:
 *   Implement the condition evaluator behavior for
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
 * File: src/runtime/bootstrap/condition_evaluator.c
 *
 * PURPOSE:
 *   Evaluate auto-configuration conditions against Framework bootstrap context.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/condition_evaluator.h"
#include "umicom/runtime/bootstrap/capability_condition.h"
#include "umicom/runtime/bootstrap/condition_set.h"
#include "umicom/runtime/bootstrap/environment_condition.h"
#include "umicom/runtime/bootstrap/feature_condition.h"
#include "umicom/runtime/bootstrap/platform_condition.h"
#include "umicom/runtime/bootstrap/property_condition.h"


/*
 * Provide the bootstrap condition evaluate operation used by this module and its client
 * applications.
 */
bool umi_bootstrap_condition_evaluate(
    const UmiBootstrapCondition *condition,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id) {
    bool result = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return false;
    /* Select the behaviour associated with the requested command or state value. */
    switch (condition->kind) {
        case UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT:
            result = umi_bootstrap_property_condition_match(properties, condition->key, NULL);
            break;
        case UMI_BOOTSTRAP_CONDITION_PROPERTY_EQUALS:
            result = umi_bootstrap_property_condition_match(properties, condition->key,
                                                             condition->expected);
            break;
        case UMI_BOOTSTRAP_CONDITION_FEATURE_PRESENT:
            result = umi_bootstrap_feature_condition_match(features, condition->key);
            break;
        case UMI_BOOTSTRAP_CONDITION_CAPABILITY_PRESENT:
            result = umi_bootstrap_capability_condition_match(capabilities, condition->key);
            break;
        case UMI_BOOTSTRAP_CONDITION_PLATFORM_EQUALS:
            result = umi_bootstrap_platform_condition_match(platform_id, condition->expected);
            break;
        case UMI_BOOTSTRAP_CONDITION_ENVIRONMENT_EQUALS:
            result = umi_bootstrap_environment_condition_match(environment, condition->key,
                                                               condition->expected);
            break;
        default:
            result = false;
            break;
    }
    return condition->negate ? !result : result;
}

/*
 * Provide the bootstrap condition set evaluate operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_condition_set_evaluate(
    const UmiBootstrapConditionSet *set,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id) {
    bool results[UMI_BOOTSTRAP_MAX_DEPENDENCIES] = {false};
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i) {
        results[i] = umi_bootstrap_condition_evaluate(&set->items[i], properties,
            environment, features, capabilities, platform_id);
    }
    return umi_bootstrap_condition_set_reduce(results, set->count, set->mode);
}
