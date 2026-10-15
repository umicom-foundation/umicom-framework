/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_context.h
 *
 * PURPOSE:
 *   Create the complete application bootstrap input owned by Framework runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_CONTEXT_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_CONTEXT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap context from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_context_init(UmiBootstrapContext *context,
                                     const char *application_id,
                                     const char *platform_id,
                                     bool dry_run);

#ifdef __cplusplus
}
#endif

#endif
