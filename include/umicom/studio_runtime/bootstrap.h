/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/bootstrap.h
 *
 * PURPOSE:
 *   Provide the single Framework bootstrap a thin Umicom Studio host needs:
 *   create the Studio runtime, register Studio composition commands, optionally
 *   enable durable semantic layout/session state, then refresh/reconcile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_BOOTSTRAP_H
#define UMICOM_STUDIO_RUNTIME_BOOTSTRAP_H

#include "umicom/studio_runtime/runtime_commands.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime bootstrap options data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeBootstrapOptions {
    const char *workspace_name;
    const char *actor_id;
    const char *session_path;
    const char *session_prefix;
    int restore_session;
} UmiStudioRuntimeBootstrapOptions;

/**
 * Represent the studio runtime bootstrap data shared with callers of this public contract.
 */
typedef struct UmiStudioRuntimeBootstrap {
    UmiStudioRuntimePlatform *platform;
    UmiStudioRuntimeSessionController *session;
    UmiStudioRuntimeCommandService *commands;
    uint64_t revision;
    int session_enabled;
    int session_restored;
} UmiStudioRuntimeBootstrap;

/**
 * Initialise studio runtime bootstrap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_runtime_bootstrap_create(
    const UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeBootstrapOptions *options,
    UmiStudioRuntimeBootstrap **out_bootstrap);

/**
 * Release or reset state held by studio runtime bootstrap so the same storage can be
 * reused safely.
 */
void umi_studio_runtime_bootstrap_destroy(
    UmiStudioRuntimeBootstrap *bootstrap);

/**
 * Provide the studio runtime bootstrap refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_bootstrap_refresh(
    UmiStudioRuntimeBootstrap *bootstrap);

/**
 * Provide the studio runtime bootstrap save session operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_runtime_bootstrap_save_session(
    UmiStudioRuntimeBootstrap *bootstrap);

#ifdef __cplusplus
}
#endif
#endif
