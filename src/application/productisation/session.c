/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/session.c
 *
 * PURPOSE:
 *   Implement a common product session so application modules only contribute
 *   identity while Framework owns layout, panel, context and readiness logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/session.h"

#include <stdio.h>
#include <string.h>

/* Treat NULL and the empty string as missing command text. */
static int has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

/* Copy identity text into bounded snapshot storage without silent truncation. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Verify the complete session chain before dereferencing its borrowed data. */
static int session_valid(const UmiProductApplicationSession *session)
{
    return session != NULL &&
        session->structure_size >= sizeof(*session) &&
        session->initialised && session->adoption != NULL &&
        session->client.contract.experience != NULL;
}

/* Check the command kind first, then require only the text used by that kind. */
static UmiStatus command_validate(
    const UmiProductApplicationSessionCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL || command->structure_size < sizeof(*command) ||
        command->kind < UMI_PRODUCT_SESSION_SELECT_LAYOUT ||
        command->kind > UMI_PRODUCT_SESSION_SYNCHRONISE_WORKBENCH)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((command->kind == UMI_PRODUCT_SESSION_SELECT_LAYOUT ||
         command->kind == UMI_PRODUCT_SESSION_ACTIVATE_PANEL ||
         command->kind == UMI_PRODUCT_SESSION_DEACTIVATE_PANEL) &&
        !has_text(command->target_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind == UMI_PRODUCT_SESSION_SET_CONTEXT &&
        (!has_text(command->target_id) || command->value == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Build adoption evidence and a thin-client runtime as one atomic session. */
UmiStatus umi_product_application_session_init(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationSession *out_session)
{
    UmiStatus status;
    /* Validate all input before clearing the caller's destination object. */
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_application_adoption_validate(adoption);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->adoption = adoption;
    status = umi_product_application_adoption_snapshot(
        adoption, &out_session->adoption_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_thin_client_init(
        adoption->application_id, &out_session->client);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_session->last_status = UMI_STATUS_OK;
    out_session->revision = 1U;
    out_session->initialised = true;
    return UMI_STATUS_OK;
}

/* Attach a presentation workbench while retaining Framework ownership of state. */
UmiStatus umi_product_application_session_bind_workbench(
    UmiProductApplicationSession *session,
    UmiUiWorkbench *workbench)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!session_valid(session) || workbench == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_thin_client_bind_workbench(
        &session->client, workbench);
    session->last_status = status;
    session->revision += 1U;
    return status;
}

/* Route one validated command to the thin client and retain auditable counters. */
UmiStatus umi_product_application_session_execute(
    UmiProductApplicationSession *session,
    const UmiProductApplicationSessionCommand *command)
{
    UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    status = command_validate(command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Each command maps to one public thin-client operation, keeping product
     * sessions independent from toolkit and application implementation details. */
    switch (command->kind) {
    case UMI_PRODUCT_SESSION_SELECT_LAYOUT:
        status = umi_application_thin_client_select_layout(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_ACTIVATE_PANEL:
        status = umi_application_thin_client_activate_panel(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_DEACTIVATE_PANEL:
        status = umi_application_thin_client_deactivate_panel(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_SET_CONTEXT:
        status = umi_application_thin_client_set_context(
            &session->client, command->target_id, command->value);
        break;
    case UMI_PRODUCT_SESSION_SET_LAYOUT_LOCKED:
        status = umi_application_thin_client_set_layout_locked(
            &session->client, command->locked);
        break;
    case UMI_PRODUCT_SESSION_REFRESH_READINESS:
        status = umi_application_thin_client_refresh_readiness(
            &session->client);
        break;
    case UMI_PRODUCT_SESSION_SYNCHRONISE_WORKBENCH:
        status = umi_application_workspace_runtime_sync_workbench(
            &session->client.workspace);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }

    /* Record both successful and failed validated commands for status panels. */
    session->command_count += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) session->successful_command_count += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else session->failed_command_count += 1U;
    session->last_status = status;
    session->revision += 1U;
    return status;
}

/* Copy current session state into a value object safe for UI and diagnostics. */
UmiStatus umi_product_application_session_snapshot(
    const UmiProductApplicationSession *session,
    UmiProductApplicationSessionSnapshot *out_snapshot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!session_valid(session) || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    status = copy_text(out_snapshot->module_id,
                       sizeof(out_snapshot->module_id),
                       session->adoption_snapshot.module_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->application_id,
                       sizeof(out_snapshot->application_id),
                       session->adoption_snapshot.application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->display_name,
                       sizeof(out_snapshot->display_name),
                       session->adoption_snapshot.display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_session_snapshot_capture(
        &session->client.workspace.session, &out_snapshot->workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->feature_count = session->adoption_snapshot.feature_count;
    out_snapshot->panel_count = session->adoption_snapshot.panel_count;
    out_snapshot->layout_count = session->adoption_snapshot.layout_count;
    out_snapshot->operation_count =
        session->client.workspace.operations.operation_count;
    out_snapshot->command_count = session->command_count;
    out_snapshot->successful_command_count =
        session->successful_command_count;
    out_snapshot->failed_command_count = session->failed_command_count;
    out_snapshot->readiness_percent = session->client.readiness.readiness_percent;
    out_snapshot->last_status = session->last_status;
    out_snapshot->revision = session->revision;
    out_snapshot->workbench_bound = session->client.workspace.workbench != NULL;
    out_snapshot->runnable = session->adoption_snapshot.runnable;
    out_snapshot->acceptance_ready =
        session->adoption_snapshot.acceptance_ready;
    return UMI_STATUS_OK;
}

/* Delegate capability probing to the same thin-client health service used by apps. */
UmiStatus umi_product_application_session_health(
    const UmiProductApplicationSession *session,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    return umi_application_thin_client_health(
        &session->client, probe, user_data, out_health);
}

/* Preserve the borrowed contribution, then rebuild every derived field. */
UmiStatus umi_product_application_session_reset(
    UmiProductApplicationSession *session)
{
    const UmiProductApplicationAdoption *adoption;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    adoption = session->adoption;
    return umi_product_application_session_init(adoption, session);
}
