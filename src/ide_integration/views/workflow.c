/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/workflow.c
 *
 * PURPOSE:
 *   Project current workflow gates and profile actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/workflow.h"

#include <stdio.h>

/*
 * Initialise ide workflow view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_workflow_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeWorkflowService *service;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    service = umi_ide_integration_platform_workflow(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_view_create_base(
        view_id,
        "ide-workflow",
        "Workflow Gates",
        "Build/test/source/debug/AI readiness gates from authoritative Framework state.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.workflow.ready", service->report.ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.workflow.gate-count",
            (int64_t)service->report.gate_count);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < service->report.gate_count;
         ++index) {
        const UmiIdeWorkflowGate *gate = &service->report.gates[index];
        char key[96];
        char row[512];

        (void)snprintf(key, sizeof(key), "ide.workflow.row.%zu", index);
        (void)snprintf(
            row, sizeof(row),
            "%.100s | %s | required=%d | %.330s",
            gate->label,
            umi_ide_gate_state_text(gate->state),
            gate->required,
            gate->detail);
        status = umi_ide_view_set_string(*out_view, key, row);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.workflow.refresh",
            "Refresh", "Refresh workflow gates.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.workflow.profile.development",
            "Development", "Use development readiness policy.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 2U, "ide.workflow.profile.pre-commit",
            "Pre-Commit", "Use pre-commit readiness policy.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 3U, "ide.workflow.profile.self-host",
            "Self-Host", "Use self-host readiness policy.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 4U, "ide.workflow.profile.release",
            "Release", "Use strict release readiness policy.", 1);

    return status;
}
