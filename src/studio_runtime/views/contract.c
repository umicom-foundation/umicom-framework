/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/contract.c
 *
 * PURPOSE:
 *   Project the final thin-Studio contract closure report.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/contract.h"

#include <stdio.h>

#include "umicom/studio_runtime/closure.h"

UmiStatus umi_studio_contract_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeBindings *bindings;
    UmiStudioRuntimeClosureReport report;
    size_t index;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_runtime_closure_check(
        umi_studio_runtime_contract(),
        bindings->shell_registry,
        bindings->commands,
        &report);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-contract",
        "Studio Contract",
        "Runtime closure of the final Framework contract for a thin Umicom Studio host.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_boolean(
        *out_view, "studio.contract.ready", report.ready);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.surfaces",
            (int64_t)report.installed_surface_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.missing-commands",
            (int64_t)report.missing_command_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.aliases",
            (int64_t)report.registered_alias_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.studio-commands",
            (int64_t)report.registered_studio_command_count);

    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < report.missing_command_count &&
         index < 24U;
         ++index) {
        char key[96];

        (void)snprintf(
            key, sizeof(key), "studio.contract.missing.%zu", index);
        status = umi_studio_view_set_string(
            *out_view,
            key,
            report.missing_commands[index]);
    }

    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 0U, "studio.contract.check",
            "Check Again", "Refresh thin-Studio contract closure.", 1);

    return status;
}
