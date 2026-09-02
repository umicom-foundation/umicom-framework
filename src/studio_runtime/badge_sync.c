/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/badge_sync.c
 *
 * PURPOSE:
 *   Implement activity/sidebar badge projection from authoritative IDE context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/badge_sync.h"

#include <limits.h>

/* Provide the bounded badge operation used by this module and its client applications. */
static uint32_t bounded_badge(size_t value)
{
    return value > UINT32_MAX ? UINT32_MAX : (uint32_t)value;
}

/* Provide the set badge operation used by this module and its client applications. */
static UmiStatus set_badge(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    uint32_t badge)
{
    UmiApplicationShellContribution contribution;
    UmiStatus status = umi_application_shell_registry_find(
        registry,
        contribution_id,
        &contribution);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_application_shell_registry_set_state(
        registry,
        contribution_id,
        (contribution.flags & UMI_APPLICATION_SHELL_VISIBLE) != 0U,
        (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
        (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
        badge);
}

/* Provide the studio badge sync operation used by this module and its client applications. */
UmiStatus umi_studio_badge_sync(
    UmiStudioRuntimeBindings *bindings,
    const UmiIdeIntegrationPlatformSnapshot *snapshot)
{
    UmiStatus status;
    size_t severe = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot->context.has_problems) {
        severe =
            snapshot->context.problems.errors +
            snapshot->context.problems.fatals;
    }

    status = set_badge(
        bindings->shell_registry,
        "umicom.shell.status-bar.problems",
        bounded_badge(severe));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_badge(
            bindings->shell_registry,
            "umicom.shell.activity-bar.source-control",
            snapshot->context.has_source_control
                ? bounded_badge(snapshot->context.source_control.change_count)
                : 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_badge(
            bindings->shell_registry,
            "umicom.shell.activity-bar.testing",
            snapshot->context.has_tests
                ? bounded_badge(snapshot->context.tests.result_count)
                : 0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_badge(
            bindings->shell_registry,
            "umicom.shell.activity-bar.ai",
            snapshot->context.has_ai
                ? bounded_badge(snapshot->context.ai.pending_approval_count)
                : 0U);
    }

    return status;
}
