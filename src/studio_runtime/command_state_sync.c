/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_state_sync.c
 *
 * PURPOSE:
 *   Implement real-command enablement reconciliation and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_state_sync.h"

#include "umicom/application_shell/command_bridge.h"

#include <string.h>

/*
 * Provide the studio runtime sync commands operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_sync_commands(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandSyncReport *out_report)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_command_bridge_reconcile(
        bindings->shell_registry,
        bindings->commands);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->contribution_count =
        umi_application_shell_registry_count(bindings->shell_registry);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_report->contribution_count; ++index) {
        UmiApplicationShellContribution contribution;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_shell_registry_at(
                bindings->shell_registry,
                index,
                &contribution) != UMI_STATUS_OK) {
            continue;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (contribution.command_id[0] == '\0') continue;

        out_report->command_backed_count += 1U;

        /* Apply this operation only while the related capability or state is available. */
        if ((contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U) {
            out_report->enabled_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_report->disabled_count += 1U;
        }
    }

    out_report->shell_revision =
        umi_application_shell_registry_revision(bindings->shell_registry);
    return UMI_STATUS_OK;
}
