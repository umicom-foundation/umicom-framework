/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/self_host.c
 *
 * PURPOSE:
 *   Project strict Umicom self-hosting readiness using the existing workflow
 *   evaluator rather than a Studio-specific checklist.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/self_host.h"
#include "umicom/ide_integration/workflow_profiles/self_host.h"

#include <stdio.h>

UmiStatus umi_ide_self_host_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeIntegrationBindings *bindings;
    UmiIdeWorkflowService *service;
    UmiIdeWorkflowPolicy policy;
    UmiIdeWorkflowReport report;
    size_t index;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_ide_integration_platform_bindings(platform);
    service = umi_ide_integration_platform_workflow(platform);
    if (bindings == NULL || service == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_workflow_profile_self_host(&policy);
    if (status == UMI_STATUS_OK) {
        status = umi_ide_workflow_evaluate(
            bindings,
            &service->context,
            &policy,
            &report);
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_create_base(
        view_id,
        "ide-self-host",
        "Self-Host Readiness",
        "Strict evidence required before Umicom Studio is trusted to build and validate itself.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.self-host.ready", report.ready);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.self-host.blocked",
            (int64_t)report.blocked_count);

    for (index = 0U; status == UMI_STATUS_OK && index < report.gate_count; ++index) {
        const UmiIdeWorkflowGate *gate = &report.gates[index];
        char key[96];
        char text[512];

        (void)snprintf(key, sizeof(key), "ide.self-host.row.%zu", index);
        (void)snprintf(
            text, sizeof(text),
            "%.100s | %s | %.350s",
            gate->label,
            umi_ide_gate_state_text(gate->state),
            gate->detail);

        status = umi_ide_view_set_string(*out_view, key, text);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.self-host.verify",
            "Verify Again", "Refresh strict self-host readiness.", 1);

    return status;
}
