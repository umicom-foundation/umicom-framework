/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/capability_condition.c
 *
 * PURPOSE:
 *   Implement the capability condition behavior for
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
 * File: src/runtime/bootstrap/capability_condition.c
 *
 * PURPOSE:
 *   Evaluate Framework runtime-capability presence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/capability_condition.h"


#include <string.h>
/*
 * Provide the bootstrap capability condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_capability_condition_match(
    const UmiBootstrapIdList *capabilities,
    const char *capability_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capabilities == NULL || capability_id == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < capabilities->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(capabilities->ids[i], capability_id) == 0) return true;
    return false;
}
