/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/contract.c
 *
 * PURPOSE:
 *   Project the final thin-Studio contract closure report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/contract.h"

#include <stdio.h>

#include "umicom/studio_runtime/closure.h"

/*
 * Initialise studio contract view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_studio_contract_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeBindings *bindings;
    UmiStudioRuntimeClosureReport report;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_runtime_closure_check(
        umi_studio_runtime_contract(),
        bindings->shell_registry,
        bindings->commands,
        &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-contract",
        "Studio Contract",
        "Runtime closure of the final Framework contract for a thin Umicom Studio host.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_boolean(
        *out_view, "studio.contract.ready", report.ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.surfaces",
            (int64_t)report.installed_surface_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.missing-commands",
            (int64_t)report.missing_command_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.aliases",
            (int64_t)report.registered_alias_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.contract.studio-commands",
            (int64_t)report.registered_studio_command_count);

    /* Visit each bounded item once so every record receives the same rule. */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 0U, "studio.contract.check",
            "Check Again", "Refresh thin-Studio contract closure.", 1);

    return status;
}
