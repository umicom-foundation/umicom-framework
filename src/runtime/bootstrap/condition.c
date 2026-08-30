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


#include <string.h>
UmiStatus umi_bootstrap_condition_init(UmiBootstrapCondition *condition,
                                       UmiBootstrapConditionKind kind,
                                       const char *key,
                                       const char *expected,
                                       bool negate) {
    UmiStatus status;
    if (condition == NULL || !umi_bootstrap_id_valid(key) ||
        kind < UMI_BOOTSTRAP_CONDITION_PROPERTY_PRESENT ||
        kind > UMI_BOOTSTRAP_CONDITION_ENVIRONMENT_EQUALS)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(condition, 0, sizeof(*condition));
    condition->kind = kind;
    status = umi_bootstrap_copy_text(condition->key, sizeof(condition->key), key);
    if (status != UMI_STATUS_OK) return status;
    if (expected != NULL) {
        status = umi_bootstrap_copy_text(condition->expected,
            sizeof(condition->expected), expected);
        if (status != UMI_STATUS_OK) return status;
    }
    condition->negate = negate;
    return UMI_STATUS_OK;
}
