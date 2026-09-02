/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workspace.c
 *
 * PURPOSE:
 *   Implement selection, control availability and filtered debugger views on
 *   top of the canonical Framework debugger registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug/workspace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugWorkspace {
    UmiDebugService *service;
    UmiDebugController *controller;
    char selected_thread_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    char selected_frame_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    char selected_scope_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    uint64_t next_watch_id;
    uint64_t revision;
    int follows_active_thread;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the clear selection after thread operation used by this module and its client
 * applications.
 */
static void clear_selection_after_thread(UmiDebugWorkspace *workspace)
{
    workspace->selected_frame_id[0] = '\0';
    workspace->selected_scope_id[0] = '\0';
}

/*
 * Provide the clear selection after frame operation used by this module and its client
 * applications.
 */
static void clear_selection_after_frame(UmiDebugWorkspace *workspace)
{
    workspace->selected_scope_id[0] = '\0';
}

/* Provide the frame is visible operation used by this module and its client applications. */
static int frame_is_visible(const UmiDebugWorkspace *workspace,
                            const UmiDebugStackFrameSnapshot *frame)
{
    return workspace->selected_thread_id[0] == '\0' ||
           strcmp(frame->thread_id, workspace->selected_thread_id) == 0;
}

/* Provide the scope is visible operation used by this module and its client applications. */
static int scope_is_visible(const UmiDebugWorkspace *workspace,
                            const UmiDebugScopeSnapshot *scope)
{
    return workspace->selected_frame_id[0] == '\0' ||
           strcmp(scope->frame_id, workspace->selected_frame_id) == 0;
}

/*
 * Provide the variable is visible operation used by this module and its client
 * applications.
 */
static int variable_is_visible(const UmiDebugWorkspace *workspace,
                               const UmiDebugVariableSnapshot *variable)
{
    return workspace->selected_scope_id[0] == '\0' ||
           strcmp(variable->scope_id, workspace->selected_scope_id) == 0;
}

/*
 * Provide the choose default thread operation used by this module and its client
 * applications.
 */
static UmiStatus choose_default_thread(UmiDebugWorkspace *workspace)
{
    UmiDebugThreadRegistry *registry;
    UmiDebugThreadSnapshot item;
    size_t index;
    size_t count;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workspace->selected_thread_id[0] != '\0') {
        return UMI_STATUS_OK;
    }
    registry = umi_debug_service_thread(workspace->service);
    count = umi_debug_thread_registry_count(registry);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_thread_registry_at(registry, index, &item) !=
            UMI_STATUS_OK) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.current) {
            return copy_text(workspace->selected_thread_id,
                             sizeof(workspace->selected_thread_id), item.id);
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 0U &&
        umi_debug_thread_registry_at(registry, 0U, &item) == UMI_STATUS_OK) {
        return copy_text(workspace->selected_thread_id,
                         sizeof(workspace->selected_thread_id), item.id);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the choose default frame operation used by this module and its client
 * applications.
 */
static UmiStatus choose_default_frame(UmiDebugWorkspace *workspace)
{
    UmiDebugStackFrameRegistry *registry;
    UmiDebugStackFrameSnapshot item;
    size_t index;
    size_t count;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workspace->selected_frame_id[0] != '\0') {
        return UMI_STATUS_OK;
    }
    registry = umi_debug_service_stack_frame(workspace->service);
    count = umi_debug_stack_frame_registry_count(registry);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_stack_frame_registry_at(registry, index, &item) ==
                UMI_STATUS_OK &&
            frame_is_visible(workspace, &item)) {
            return copy_text(workspace->selected_frame_id,
                             sizeof(workspace->selected_frame_id), item.id);
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the choose default scope operation used by this module and its client
 * applications.
 */
static UmiStatus choose_default_scope(UmiDebugWorkspace *workspace)
{
    UmiDebugScopeRegistry *registry;
    UmiDebugScopeSnapshot item;
    size_t index;
    size_t count;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workspace->selected_scope_id[0] != '\0') {
        return UMI_STATUS_OK;
    }
    registry = umi_debug_service_scope(workspace->service);
    count = umi_debug_scope_registry_count(registry);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_scope_registry_at(registry, index, &item) ==
                UMI_STATUS_OK &&
            scope_is_visible(workspace, &item)) {
            return copy_text(workspace->selected_scope_id,
                             sizeof(workspace->selected_scope_id), item.id);
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise debug workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_workspace_create(UmiDebugService *service,
                                     UmiDebugController *controller,
                                     UmiDebugWorkspace **out_workspace)
{
    UmiDebugWorkspace *workspace;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || controller == NULL || out_workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workspace = NULL;
    workspace = (UmiDebugWorkspace *)calloc(1U, sizeof(*workspace));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    workspace->service = service;
    workspace->controller = controller;
    workspace->next_watch_id = 1U;
    workspace->revision = 1U;
    workspace->follows_active_thread = 1;
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

/* Release or reset state held by debug workspace so the same storage can be reused safely. */
void umi_debug_workspace_destroy(UmiDebugWorkspace *workspace)
{
    free(workspace);
}

/*
 * Provide the debug workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_refresh(UmiDebugWorkspace *workspace)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = choose_default_thread(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = choose_default_frame(workspace);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = choose_default_scope(workspace);
    }
    return status;
}

/* Return the number of records represented by visible frame without changing their state. */
static size_t visible_frame_count(UmiDebugWorkspace *workspace)
{
    UmiDebugStackFrameSnapshot item;
    size_t index;
    size_t count = 0U;
    UmiDebugStackFrameRegistry *registry =
        umi_debug_service_stack_frame(workspace->service);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_stack_frame_registry_count(registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_stack_frame_registry_at(registry, index, &item) ==
                UMI_STATUS_OK &&
            frame_is_visible(workspace, &item)) {
            count += 1U;
        }
    }
    return count;
}

/* Return the number of records represented by visible scope without changing their state. */
static size_t visible_scope_count(UmiDebugWorkspace *workspace)
{
    UmiDebugScopeSnapshot item;
    size_t index;
    size_t count = 0U;
    UmiDebugScopeRegistry *registry =
        umi_debug_service_scope(workspace->service);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_scope_registry_count(registry); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_scope_registry_at(registry, index, &item) ==
                UMI_STATUS_OK &&
            scope_is_visible(workspace, &item)) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Return the number of records represented by visible variable without changing their
 * state.
 */
static size_t visible_variable_count(UmiDebugWorkspace *workspace)
{
    UmiDebugVariableSnapshot item;
    size_t index;
    size_t count = 0U;
    UmiDebugVariableRegistry *registry =
        umi_debug_service_variable(workspace->service);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_variable_registry_count(registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_variable_registry_at(registry, index, &item) ==
                UMI_STATUS_OK &&
            variable_is_visible(workspace, &item)) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Provide the debug workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_snapshot(UmiDebugWorkspace *workspace,
                                       UmiDebugWorkspaceSnapshot *out_snapshot)
{
    UmiDebugControllerSnapshot controller;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_workspace_refresh(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_debug_controller_snapshot(workspace->controller, &controller);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_WORKSPACE_API_VERSION;
    out_snapshot->revision = workspace->revision > controller.revision
        ? workspace->revision : controller.revision;
    out_snapshot->controller_state = controller.state;
    (void)snprintf(out_snapshot->controller_state_label,
                   sizeof(out_snapshot->controller_state_label), "%s",
                   umi_debug_controller_state_text(controller.state));
    (void)snprintf(out_snapshot->selected_thread_id,
                   sizeof(out_snapshot->selected_thread_id), "%s",
                   workspace->selected_thread_id);
    (void)snprintf(out_snapshot->selected_frame_id,
                   sizeof(out_snapshot->selected_frame_id), "%s",
                   workspace->selected_frame_id);
    (void)snprintf(out_snapshot->selected_scope_id,
                   sizeof(out_snapshot->selected_scope_id), "%s",
                   workspace->selected_scope_id);
    out_snapshot->thread_count = umi_debug_thread_registry_count(
        umi_debug_service_thread(workspace->service));
    out_snapshot->visible_frame_count = visible_frame_count(workspace);
    out_snapshot->visible_scope_count = visible_scope_count(workspace);
    out_snapshot->visible_variable_count = visible_variable_count(workspace);
    out_snapshot->watch_count = umi_debug_watch_registry_count(
        umi_debug_service_watch(workspace->service));
    out_snapshot->breakpoint_count = umi_debug_breakpoint_registry_count(
        umi_debug_service_breakpoint(workspace->service));
    out_snapshot->console_entry_count = umi_debug_console_entry_registry_count(
        umi_debug_service_console_entry(workspace->service));
    out_snapshot->can_start = controller.state == UMI_DEBUG_CONTROLLER_IDLE ||
                              controller.state == UMI_DEBUG_CONTROLLER_TERMINATED;
    out_snapshot->can_continue = controller.state == UMI_DEBUG_CONTROLLER_PAUSED;
    out_snapshot->can_pause = controller.state == UMI_DEBUG_CONTROLLER_RUNNING;
    out_snapshot->can_step = controller.state == UMI_DEBUG_CONTROLLER_PAUSED;
    out_snapshot->can_stop = controller.state != UMI_DEBUG_CONTROLLER_IDLE &&
                             controller.state != UMI_DEBUG_CONTROLLER_TERMINATED;
    out_snapshot->follows_active_thread = workspace->follows_active_thread;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workspace select thread operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_thread(UmiDebugWorkspace *workspace,
                                            const char *thread_id)
{
    UmiDebugThreadSnapshot item;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || thread_id == NULL || thread_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_thread_registry_find(
        umi_debug_service_thread(workspace->service), thread_id, &item);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->selected_thread_id,
                           sizeof(workspace->selected_thread_id), item.id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        clear_selection_after_thread(workspace);
        workspace->revision += 1U;
        status = umi_debug_workspace_refresh(workspace);
    }
    return status;
}

/*
 * Provide the debug workspace select frame operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_frame(UmiDebugWorkspace *workspace,
                                           const char *frame_id)
{
    UmiDebugStackFrameSnapshot item;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || frame_id == NULL || frame_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_stack_frame_registry_find(
        umi_debug_service_stack_frame(workspace->service), frame_id, &item);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !frame_is_visible(workspace, &item)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->selected_frame_id,
                           sizeof(workspace->selected_frame_id), item.id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        clear_selection_after_frame(workspace);
        workspace->revision += 1U;
        status = umi_debug_workspace_refresh(workspace);
    }
    return status;
}

/*
 * Provide the debug workspace select scope operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_scope(UmiDebugWorkspace *workspace,
                                           const char *scope_id)
{
    UmiDebugScopeSnapshot item;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || scope_id == NULL || scope_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_scope_registry_find(
        umi_debug_service_scope(workspace->service), scope_id, &item);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && !scope_is_visible(workspace, &item)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(workspace->selected_scope_id,
                           sizeof(workspace->selected_scope_id), item.id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug workspace set follow active thread operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workspace_set_follow_active_thread(
    UmiDebugWorkspace *workspace, int enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workspace->follows_active_thread = enabled != 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workspace add watch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_add_watch(UmiDebugWorkspace *workspace,
                                        const char *expression,
                                        char *out_watch_id,
                                        size_t out_watch_id_capacity)
{
    UmiDebugWatchSnapshot watch = {0};
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || expression == NULL || expression[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    watch.struct_size = (uint32_t)sizeof(watch);
    watch.api_version = UMI_DEBUG_WATCH_API_VERSION;
    (void)snprintf(watch.id, sizeof(watch.id), "watch-%llu",
                   (unsigned long long)workspace->next_watch_id);
    status = copy_text(watch.expression, sizeof(watch.expression), expression);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)snprintf(watch.value, sizeof(watch.value), "%s", "Not evaluated");
    watch.enabled = 1;
    watch.valid = 1;
    status = umi_debug_watch_registry_upsert(
        umi_debug_service_watch(workspace->service), &watch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workspace->next_watch_id += 1U;
        workspace->revision += 1U;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_watch_id != NULL && out_watch_id_capacity > 0U) {
            status = copy_text(out_watch_id, out_watch_id_capacity, watch.id);
        }
    }
    return status;
}

/*
 * Provide the debug workspace remove watch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_remove_watch(UmiDebugWorkspace *workspace,
                                           const char *watch_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || watch_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_watch_registry_remove(
        umi_debug_service_watch(workspace->service), watch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug workspace clear watches operation used by this module and its client
 * applications.
 */
void umi_debug_workspace_clear_watches(UmiDebugWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace != NULL) {
        umi_debug_watch_registry_clear(
            umi_debug_service_watch(workspace->service));
        workspace->revision += 1U;
    }
}

/*
 * Provide the debug workspace set breakpoint enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workspace_set_breakpoint_enabled(
    UmiDebugWorkspace *workspace, const char *breakpoint_id, int enabled)
{
    UmiDebugBreakpointSnapshot breakpoint;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || breakpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_breakpoint_registry_find(
        umi_debug_service_breakpoint(workspace->service), breakpoint_id,
        &breakpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        breakpoint.enabled = enabled != 0;
        status = umi_debug_breakpoint_registry_upsert(
            umi_debug_service_breakpoint(workspace->service), &breakpoint);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug workspace remove breakpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workspace_remove_breakpoint(
    UmiDebugWorkspace *workspace, const char *breakpoint_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || breakpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_breakpoint_registry_remove(
        umi_debug_service_breakpoint(workspace->service), breakpoint_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/*
 * Provide the debug workspace clear breakpoints operation used by this module and its
 * client applications.
 */
void umi_debug_workspace_clear_breakpoints(UmiDebugWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace != NULL) {
        umi_debug_breakpoint_registry_clear(
            umi_debug_service_breakpoint(workspace->service));
        workspace->revision += 1U;
    }
}

/*
 * Provide the debug workspace clear console operation used by this module and its client
 * applications.
 */
void umi_debug_workspace_clear_console(UmiDebugWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace != NULL) {
        umi_debug_console_entry_registry_clear(
            umi_debug_service_console_entry(workspace->service));
        workspace->revision += 1U;
    }
}

/*
 * Find debug workspace thread while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_thread_at(UmiDebugWorkspace *workspace,
                                        size_t index,
                                        UmiDebugThreadSnapshot *out_thread)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_thread == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_debug_thread_registry_at(
        umi_debug_service_thread(workspace->service), index, out_thread);
}

/*
 * Find debug workspace frame while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_frame_at(UmiDebugWorkspace *workspace,
                                       size_t visible_index,
                                       UmiDebugStackFrameSnapshot *out_frame)
{
    UmiDebugStackFrameSnapshot item;
    UmiDebugStackFrameRegistry *registry;
    size_t index;
    size_t matched = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_frame == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    registry = umi_debug_service_stack_frame(workspace->service);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_stack_frame_registry_count(registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_stack_frame_registry_at(registry, index, &item) !=
                UMI_STATUS_OK ||
            !frame_is_visible(workspace, &item)) {
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched++ == visible_index) {
            *out_frame = item;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find debug workspace scope while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_scope_at(UmiDebugWorkspace *workspace,
                                       size_t visible_index,
                                       UmiDebugScopeSnapshot *out_scope)
{
    UmiDebugScopeSnapshot item;
    UmiDebugScopeRegistry *registry;
    size_t index;
    size_t matched = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_scope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    registry = umi_debug_service_scope(workspace->service);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_scope_registry_count(registry); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_scope_registry_at(registry, index, &item) !=
                UMI_STATUS_OK ||
            !scope_is_visible(workspace, &item)) {
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched++ == visible_index) {
            *out_scope = item;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find debug workspace variable while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_variable_at(UmiDebugWorkspace *workspace,
                                          size_t visible_index,
                                          UmiDebugVariableSnapshot *out_variable)
{
    UmiDebugVariableSnapshot item;
    UmiDebugVariableRegistry *registry;
    size_t index;
    size_t matched = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_variable == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    registry = umi_debug_service_variable(workspace->service);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_variable_registry_count(registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_debug_variable_registry_at(registry, index, &item) !=
                UMI_STATUS_OK ||
            !variable_is_visible(workspace, &item)) {
            continue;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched++ == visible_index) {
            *out_variable = item;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find debug workspace watch while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_watch_at(UmiDebugWorkspace *workspace,
                                       size_t index,
                                       UmiDebugWatchSnapshot *out_watch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_watch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_debug_watch_registry_at(
        umi_debug_service_watch(workspace->service), index, out_watch);
}

/*
 * Find debug workspace breakpoint while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_breakpoint_at(
    UmiDebugWorkspace *workspace, size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_breakpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_debug_breakpoint_registry_at(
        umi_debug_service_breakpoint(workspace->service), index,
        out_breakpoint);
}

/*
 * Find debug workspace console entry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_workspace_console_entry_at(
    UmiDebugWorkspace *workspace, size_t index,
    UmiDebugConsoleEntrySnapshot *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_debug_console_entry_registry_at(
        umi_debug_service_console_entry(workspace->service), index, out_entry);
}
