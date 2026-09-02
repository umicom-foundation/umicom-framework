/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/alias_binding.c
 *
 * PURPOSE:
 *   Implement the alias binding behavior for
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
 * File: src/runtime/bootstrap/alias_binding.c
 *
 * PURPOSE:
 *   Map alternate service identifiers without copying or owning instances.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/alias_binding.h"


#include <string.h>
/*
 * Provide the bootstrap alias binding configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_alias_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    const char *target_service_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || !umi_bootstrap_id_valid(target_service_id) ||
        strcmp(binding->key.service_id, target_service_id) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_bootstrap_copy_text(binding->target_id,
        sizeof(binding->target_id), target_service_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    binding->kind = UMI_BOOTSTRAP_BIND_ALIAS;
    binding->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that bootstrap alias binding satisfies its contract before another service relies
 * on it.
 */
bool umi_bootstrap_alias_binding_valid(
    const UmiBootstrapBindingDescriptor *binding) {
    return binding != NULL && binding->kind == UMI_BOOTSTRAP_BIND_ALIAS &&
           umi_bootstrap_id_valid(binding->target_id) &&
           strcmp(binding->key.service_id, binding->target_id) != 0;
}
