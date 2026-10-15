/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/lifecycle_hook.h
 *
 * PURPOSE:
 *   Create lifecycle hooks bound to a service and ordered runtime phase.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_HOOK_H
#define UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_HOOK_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap lifecycle hook from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_lifecycle_hook_init(UmiBootstrapLifecycleHook *hook,
                                            const char *hook_id,
                                            const char *service_id,
                                            UmiBootstrapLifecyclePhase phase,
                                            int32_t order);

#ifdef __cplusplus
}
#endif

#endif
