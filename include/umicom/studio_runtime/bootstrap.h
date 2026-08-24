/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/bootstrap.h
 *
 * PURPOSE:
 *   Provide the single Framework bootstrap a thin Umicom Studio host needs:
 *   create the Studio runtime, register Studio composition commands, optionally
 *   enable durable semantic layout/session state, then refresh/reconcile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_BOOTSTRAP_H
#define UMICOM_STUDIO_RUNTIME_BOOTSTRAP_H

#include "umicom/studio_runtime/runtime_commands.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeBootstrapOptions {
    const char *workspace_name;
    const char *actor_id;
    const char *session_path;
    const char *session_prefix;
    int restore_session;
} UmiStudioRuntimeBootstrapOptions;

typedef struct UmiStudioRuntimeBootstrap {
    UmiStudioRuntimePlatform *platform;
    UmiStudioRuntimeSessionController *session;
    UmiStudioRuntimeCommandService *commands;
    uint64_t revision;
    int session_enabled;
    int session_restored;
} UmiStudioRuntimeBootstrap;

UmiStatus umi_studio_runtime_bootstrap_create(
    const UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeBootstrapOptions *options,
    UmiStudioRuntimeBootstrap **out_bootstrap);

void umi_studio_runtime_bootstrap_destroy(
    UmiStudioRuntimeBootstrap *bootstrap);

UmiStatus umi_studio_runtime_bootstrap_refresh(
    UmiStudioRuntimeBootstrap *bootstrap);

UmiStatus umi_studio_runtime_bootstrap_save_session(
    UmiStudioRuntimeBootstrap *bootstrap);

#ifdef __cplusplus
}
#endif
#endif
