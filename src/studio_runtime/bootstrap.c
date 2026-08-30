/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/bootstrap.c
 *
 * PURPOSE:
 *   Implement one-call Framework Studio runtime bootstrap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/bootstrap.h"

#include <stdlib.h>

UmiStatus umi_studio_runtime_bootstrap_create(
    const UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeBootstrapOptions *options,
    UmiStudioRuntimeBootstrap **out_bootstrap)
{
    UmiStudioRuntimeBootstrap *bootstrap;
    UmiStatus status;

    if (bindings == NULL || options == NULL ||
        options->workspace_name == NULL ||
        options->actor_id == NULL ||
        out_bootstrap == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bootstrap = NULL;

    bootstrap = (UmiStudioRuntimeBootstrap *)calloc(
        1U, sizeof(*bootstrap));
    if (bootstrap == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_studio_runtime_platform_create(
        bindings,
        options->workspace_name,
        options->actor_id,
        &bootstrap->platform);
    if (status != UMI_STATUS_OK) goto fail;

    if (options->session_path != NULL &&
        options->session_path[0] != '\0' &&
        options->session_prefix != NULL &&
        options->session_prefix[0] != '\0') {
        status = umi_studio_session_controller_create(
            options->session_path,
            options->session_prefix,
            &bootstrap->session);
        if (status != UMI_STATUS_OK) goto fail;

        bootstrap->session_enabled = 1;
    }

    status = umi_studio_runtime_command_service_create(
        bootstrap->platform,
        bootstrap->session,
        &bootstrap->commands);
    if (status != UMI_STATUS_OK) goto fail;

    status = umi_studio_runtime_command_service_register(
        bootstrap->commands);
    if (status != UMI_STATUS_OK) goto fail;

    if (bootstrap->session_enabled && options->restore_session) {
        int restored = 0;

        status = umi_studio_session_controller_restore(
            bootstrap->session,
            bootstrap->platform,
            &restored);

        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            goto fail;
        }

        bootstrap->session_restored = restored;
        status = UMI_STATUS_OK;
    }

    status = umi_studio_runtime_platform_refresh(
        bootstrap->platform);
    if (status != UMI_STATUS_OK) goto fail;

    bootstrap->revision = 1U;
    *out_bootstrap = bootstrap;
    return UMI_STATUS_OK;

fail:
    umi_studio_runtime_bootstrap_destroy(bootstrap);
    return status;
}

void umi_studio_runtime_bootstrap_destroy(
    UmiStudioRuntimeBootstrap *bootstrap)
{
    if (bootstrap == NULL) return;

    umi_studio_runtime_command_service_destroy(bootstrap->commands);
    umi_studio_session_controller_destroy(bootstrap->session);
    umi_studio_runtime_platform_destroy(bootstrap->platform);
    free(bootstrap);
}

UmiStatus umi_studio_runtime_bootstrap_refresh(
    UmiStudioRuntimeBootstrap *bootstrap)
{
    UmiStatus status;

    if (bootstrap == NULL || bootstrap->platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_platform_refresh(
        bootstrap->platform);
    if (status == UMI_STATUS_OK) bootstrap->revision += 1U;
    return status;
}

UmiStatus umi_studio_runtime_bootstrap_save_session(
    UmiStudioRuntimeBootstrap *bootstrap)
{
    UmiStatus status;

    if (bootstrap == NULL ||
        !bootstrap->session_enabled ||
        bootstrap->session == NULL ||
        bootstrap->platform == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_studio_session_controller_save(
        bootstrap->session,
        bootstrap->platform);
    if (status == UMI_STATUS_OK) bootstrap->revision += 1U;
    return status;
}
