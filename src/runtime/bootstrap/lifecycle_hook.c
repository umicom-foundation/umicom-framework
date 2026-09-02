/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/lifecycle_hook.c
 *
 * PURPOSE:
 *   Implement the lifecycle hook behavior for
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
 * File: src/runtime/bootstrap/lifecycle_hook.c
 *
 * PURPOSE:
 *   Create lifecycle hooks bound to a service and ordered runtime phase.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/lifecycle_hook.h"
#include "umicom/runtime/bootstrap/lifecycle_phase.h"


#include <string.h>
/*
 * Initialise bootstrap lifecycle hook from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_lifecycle_hook_init(UmiBootstrapLifecycleHook *hook,
                                            const char *hook_id,
                                            const char *service_id,
                                            UmiBootstrapLifecyclePhase phase,
                                            int32_t order) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hook == NULL || !umi_bootstrap_id_valid(hook_id) ||
        !umi_bootstrap_id_valid(service_id) ||
        umi_bootstrap_lifecycle_phase_rank(phase) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(hook, 0, sizeof(*hook));
    status = umi_bootstrap_copy_text(hook->hook_id, sizeof(hook->hook_id), hook_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(hook->service_id, sizeof(hook->service_id), service_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    hook->phase = phase;
    hook->order = order;
    return UMI_STATUS_OK;
}
