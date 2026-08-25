/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/capability_condition.c
 *
 * PURPOSE:
 *   Evaluate Framework runtime-capability presence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/capability_condition.h"


#include <string.h>
bool umi_bootstrap_capability_condition_match(
    const UmiBootstrapIdList *capabilities,
    const char *capability_id) {
    size_t i;
    if (capabilities == NULL || capability_id == NULL) return false;
    for (i = 0U; i < capabilities->count; ++i)
        if (strcmp(capabilities->ids[i], capability_id) == 0) return true;
    return false;
}
