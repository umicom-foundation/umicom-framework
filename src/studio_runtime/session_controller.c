/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/session_controller.c
 *
 * PURPOSE:
 *   Implement durable semantic Studio shell/layout restoration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/session_controller.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_studio_session_controller_create(
    const char *path,
    const char *prefix,
    UmiStudioRuntimeSessionController **out_controller)
{
    UmiStudioRuntimeSessionController *controller;
    size_t path_length;
    size_t prefix_length;
    UmiStatus status;

    if (path == NULL || path[0] == '\0' ||
        prefix == NULL || prefix[0] == '\0' ||
        out_controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path_length = strlen(path);
    prefix_length = strlen(prefix);

    if (path_length >= UMI_STUDIO_RUNTIME_PATH_CAPACITY ||
        prefix_length >= UMI_SESSION_KEY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    *out_controller = NULL;

    controller = (UmiStudioRuntimeSessionController *)calloc(
        1U, sizeof(*controller));
    if (controller == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_session_store_create(&controller->store);
    if (status != UMI_STATUS_OK) {
        free(controller);
        return status;
    }

    (void)memcpy(controller->path, path, path_length + 1U);
    (void)memcpy(controller->prefix, prefix, prefix_length + 1U);
    controller->revision = 1U;

    *out_controller = controller;
    return UMI_STATUS_OK;
}

void umi_studio_session_controller_destroy(
    UmiStudioRuntimeSessionController *controller)
{
    if (controller == NULL) return;
    umi_session_store_destroy(controller->store);
    free(controller);
}

UmiStatus umi_studio_session_controller_save(
    UmiStudioRuntimeSessionController *controller,
    UmiStudioRuntimePlatform *platform)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStudioRuntimeBindings *bindings;
    UmiStatus status;

    if (controller == NULL || controller->store == NULL ||
        platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_runtime_platform_snapshot(
        platform,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_layout_session_save(
        controller->store,
        controller->prefix,
        snapshot.active_layout_preset_id,
        bindings->shell_state,
        bindings->shell_layout);
    if (status != UMI_STATUS_OK) return status;

    status = umi_session_store_save(
        controller->store,
        controller->path);
    if (status == UMI_STATUS_OK) controller->revision += 1U;
    return status;
}

UmiStatus umi_studio_session_controller_restore(
    UmiStudioRuntimeSessionController *controller,
    UmiStudioRuntimePlatform *platform,
    int *out_restored)
{
    UmiStudioRuntimeBindings *bindings;
    UmiApplicationShellState restored_state;
    UmiApplicationShellLayout restored_layout;
    char preset_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    int loaded = 0;
    int restored = 0;
    UmiStatus status;

    if (controller == NULL || controller->store == NULL ||
        platform == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;

    status = umi_session_store_load(
        controller->store,
        controller->path,
        &loaded);
    if (status != UMI_STATUS_OK) return status;

    controller->loaded = loaded;
    if (!loaded) return UMI_STATUS_OK;

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    umi_application_shell_state_init(&restored_state);
    umi_application_shell_layout_init(
        &restored_layout,
        "umicom.studio.layout.restored",
        "Restored Layout");

    status = umi_studio_layout_session_restore(
        controller->store,
        controller->prefix,
        preset_id,
        sizeof(preset_id),
        &restored_state,
        &restored_layout,
        &restored);
    if (status != UMI_STATUS_OK || !restored) return status;

    /*
     * Apply the known preset first so new contributions introduced by a newer
     * Framework still receive sensible defaults. Then overlay the saved semantic
     * layout/state. This is forward-compatible without serializing widgets.
     */
    if (umi_studio_layout_catalogue_find(preset_id) != NULL) {
        status = umi_studio_runtime_platform_select_layout(
            platform,
            preset_id);
        if (status != UMI_STATUS_OK) return status;
    }

    *bindings->shell_state = restored_state;
    *bindings->shell_layout = restored_layout;

    status = umi_studio_runtime_platform_refresh(platform);
    if (status != UMI_STATUS_OK) return status;

    controller->revision += 1U;
    *out_restored = 1;
    return UMI_STATUS_OK;
}
